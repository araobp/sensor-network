#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c1_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "BACKPLANE-MASTER"

// Backplane-master-specific commands
#define I2C "I2C"
#define MAP "MAP"
#define STS "STS"


// Debug commands
#define DEV "DEV"
#define REG "REG"
#define RED "RED"
#define WRT "WRT"
#define SEN "SEN"

#define MAX_DEV_ADDR 48  // it must be larger than 16 and multipels of 8
#define MASK 0x01

// Time slots
#define S_PLG 0x00  // detect new device ([pl]u[g]-in) 
#define S_ADT 0x01  // periodic [a]u[d]i[t] (preserved)
#define S_INV 0x02  // [inv]oke device to execute command and fetch sensor data
#define S_NOP 0x03  // No operation
#define S_CMD 0x04  // execute [c]o[m]man[d]
#define S_ACP 0x05  // [ac]ce[p]t command

#define INV_SEN_DELAY 3  // INV -> SEN delay

const uint8_t MAX_Y = MAX_DEV_ADDR/8;

bool running = false;
uint8_t timer_cnt = 0;
bool do_func = false;
uint8_t read_buf[16];
uint8_t dev_map[MAX_Y];  // I2C slave device map

void exec_remote_cmd(uint8_t idx);

void start_handler(void) {
    running = true;
}

void stop_handler(void) {
    running = false;
}

void clear_dev_map(void) {
    uint8_t y;
    for (y=0; y<MAX_Y; y++) {
        dev_map[y] = 0;
    }
}

uint8_t devs = 0;
        
void add_dev(uint8_t dev_addr) {
    uint8_t x, y;
    uint8_t new_map, old_map;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        old_map = dev_map[y];
        new_map = dev_map[y] | (0x01 << x);
        if (old_map != new_map) {
            dev_map[y] = new_map;
            devs++;
        }
    }
}

void del_dev(uint8_t dev_addr) {
    uint8_t x, y;
    uint8_t new_map, old_map;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        old_map = dev_map[y];
        new_map = dev_map[y] & ~(0x01 << x);
        if (old_map != new_map) {
            dev_map[y] = new_map;
            devs--;
        }
   }
}

uint8_t dev_map_iterator() {
    static uint8_t xx = 0;
    static uint8_t yy = 0;
    static bool start = true;
    bool exist = false;
    uint8_t dev_addr;

    if (devs) {
        while(1) {
            if (xx > 7) {
                xx = 0;
                yy++;
            }
            if (yy >= MAX_Y) {
                xx = 0;
                yy = 0;
            }
            if ((MASK << xx) & dev_map[yy]) {
                dev_addr = yy * 8 + xx++;
                break;
            }
            xx++;
        }
    }
    return dev_addr;
}

void scan_dev(void) {
    uint8_t dev_addr, status;
    for (dev_addr=1; dev_addr<=MAX_DEV_ADDR; dev_addr++) {
        status = i2c1_read(dev_addr, WHO_I2C, &read_buf[0], 1);
        // printf("%d %d\n", dev_addr, status);
        if (status == 0) {
            add_dev(read_buf[0]);
        }
    }  
}

bool detected(uint8_t dev_addr) {
    uint8_t y = dev_addr / 8;
    uint8_t x = dev_addr % 8;
    bool exist = false;
    if ((dev_map[y] & (0x01 << x)) > 0) {
        exist = true;
    }
    return exist;
}

void print_dev_map(void) {
    uint8_t i;
    uint8_t len = devs;
    if (len > 0) {
        len--;
        printf("$:MAP:");
        for (i=0; i<len; i++) printf("%d,", dev_map_iterator());
        printf("%d\n", dev_map_iterator());
    } else {
        printf("!:NO SLAVE FOUND\n");
    }
}

uint8_t sen(uint8_t dev_addr) {
    uint8_t status;
    uint8_t type;
    uint8_t length ,data, i;
    LATCbits.LATC7 ^= 1;
    status = i2c1_read(dev_addr, SEN_I2C, &type, 1);
    if (status == 0) {
        if (type == TYPE_NO_DATA) {
                PROTOCOL_Print_TLV(dev_addr, TYPE_NO_DATA, 0, NULL);            
        } else {
            status = i2c1_read(dev_addr, SEN_I2C, &length, 1);
            if (status == 0) {
                status = i2c1_read(dev_addr, SEN_I2C, &read_buf[0], length);
                if (status == 0) {
                    PROTOCOL_Print_TLV(dev_addr, type, length, &read_buf[0]);
                }
            }
        }                    
    }   
    return status;
}

uint8_t schedule[20][2] = {
    {S_PLG, 0},               // 0
    {S_ACP, 0},               // 1
    {S_CMD, 0},               // 2
    {S_INV, HDC1000_I2C},     // 3
    {S_ADT, 0},               // 4
    {S_INV, A1324LUA_T_I2C},  // 5
    {S_NOP, 0},               // 6
    {S_INV, KXR94_2050_I2C},  // 7
    {S_NOP, 0},               // 8
    {S_NOP, 0},               // 9
    {S_ACP, 0},               // 10
    {S_CMD, 0},               // 11
    {S_NOP, 0},               // 12
    {S_NOP, 0},               // 13
    {S_NOP, 0},               // 14
    {S_NOP, 0},               // 15
    {S_NOP, 0},               // 16
    {S_INV, KXR94_2050_I2C},  // 17
    {S_NOP, 0},               // 18
    {S_NOP, 0}                // 19
};

uint8_t current[2];
uint8_t position = 0;

void inv_handler(void) {
    uint8_t dev_addr, status;
    switch(schedule[position][0]) {
        case S_PLG:
            status = i2c1_write_no_data(GENERAL_CALL_ADDRESS, PLG_I2C);
            if (status == 0) scan_dev();
            break;
        case S_ADT:
            break;
        case S_CMD:
            if (schedule[position][1] == 1) {
                exec_remote_cmd((position == 2) ? 0 : 1);
                schedule[position][1] = 0;
            }
            break;
        case S_INV:
            dev_addr = schedule[position][1];
            if (dev_addr && detected(dev_addr)) {
                i2c1_write_no_data(dev_addr, INV_I2C);
                __delay_ms(INV_SEN_DELAY);
                status = sen(dev_addr);
                // if (status > 0) del_dev(dev_addr);
                if (status > 0) {
                    printf("!:%%%d:NETWORK_ERROR\n", dev_addr);
                    i2c1_write_no_data(dev_addr, RST_I2C);
                }   
            }
            break;
        case S_ACP:
            printf("*\n");
            break;
    }
    if (++position > 20) position = 0;
}

char cmd_buf[2][BUF_SIZE];
uint8_t idx = 0;

void put_cmd(uint8_t *buf) {
    strcpy(cmd_buf[idx], buf);
    switch(idx) {
        case 0:
            schedule[2][1] = 1;
            break;
        case 1:
            schedule[11][1] = 1;
            break;
    }       
    if (++idx > 1) idx = 0;
}

void extension_handler(uint8_t *buf) {
    if (!strncmp(I2C, buf, 3)) {
        BACKPLANE_SLAVE_ADDRESS = atoi(&buf[4]);
    } else if (!strncmp(MAP, buf, 3)) {
        print_dev_map();
    } else if (BACKPLANE_SLAVE_ADDRESS != BACKPLANE_MASTER_I2C) {
        put_cmd(buf);
    }
}

void exec_remote_cmd(uint8_t idx) {
    uint8_t data;
    uint8_t type;
    uint8_t i, j;
    uint8_t status;

    /***** For debug commands *****/
    static uint8_t dev_addr;
    static uint8_t reg_addr;

    char *buf = cmd_buf[idx];
    uint8_t send_buf[BUF_SIZE + 2];
    
    if (!strncmp(WHO, buf, 3)) {
        status = i2c1_read(BACKPLANE_SLAVE_ADDRESS, WHO_I2C, &data, 1);
        if (status == 0) printf("$:WHO:%d\n", data);
        else printf("!\n");        
    } else if (!strncmp(SAV, buf, 3)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, SAV_I2C);
    } else if (!strncmp(STA, buf, 3)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, STA_I2C);
    } else if (!strncmp(STP, buf, 3)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, STP_I2C);
        if (status == 0) printf("*:STP:ACK\n");
        else printf("!:STP:NACK\n");
    } else if (!strncmp(SET, buf, 3)) {
        data = atoi(&buf[4]);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, SET_I2C);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, data);        
    } else if (!strncmp(GET, buf, 3)) {
        i2c1_read(BACKPLANE_SLAVE_ADDRESS, GET_I2C, &data, 1);
        printf("$:GET:%d\n", data);
    } else if (!strncmp(SEN, buf, 3)) {
        if (sen(BACKPLANE_SLAVE_ADDRESS) > 0) {
            printf("!:SEN:DATA NOT READY\n");
        }
    /* Extended commands */
    } else {
        i = 0;
        j = 2;
        do {
            send_buf[j++] = (uint8_t)buf[i];
        } while (buf[i++] != '\0');
        send_buf[0] = EXT_I2C;
        send_buf[1] = i;
        status = i2c1_write(BACKPLANE_SLAVE_ADDRESS, send_buf, j);
        if (status != 0) printf("!:NO ACK FROM I2C SLAVE\n");
        // printf("\n");
    }
}

void main(void)
{
    clear_dev_map();

    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    I2C1_Initialize();
    
    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, NULL, inv_handler, 1);
    PROTOCOL_Set_Extension_Handler(extension_handler);
    
    scan_dev();

    PROTOCOL_Loop();
}