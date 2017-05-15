#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c1_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "BACKPLANE_MASTER"

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

#define S_PLG 0x00
#define S_ADT 0x01
#define S_INV 0x02
#define S_SEN 0x03
#define S_CMD 0x04

const uint8_t MAX_Y = MAX_DEV_ADDR/8;

bool running = true;
uint8_t timer_cnt = 0;
bool do_func = false;
uint8_t read_buf[16];
uint8_t dev_map[MAX_Y];  // I2C slave device map

uint8_t dev_map_iterator(void);
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

uint8_t numbers_of_dev(void) {
    uint8_t num = 0;
    uint8_t x, y;
    for (y=0; y<MAX_Y; y++) {
        for (x=0; x<8; x++) {
            if ((dev_map[y] & (0b00000001 << x)) > 0) ++num;
        }
    }  
    return num;
}

void add_dev(uint8_t dev_addr) {
    uint8_t x, y;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        dev_map[y] = dev_map[y] | (0x01 << x);
    }
}

void del_dev(uint8_t dev_addr) {
    uint8_t x, y;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        dev_map[y] = dev_map[y] & ~(0x01 << x);
    }
}

void print_dev_map(void) {
    uint8_t i;
    uint8_t len = numbers_of_dev();
    if (len > 0) {
        len--;
        printf("$:MAP:");
        for (i=0; i<len; i++) printf("%d,", dev_map_iterator());
        printf("%d\n", dev_map_iterator());
    } else {
        printf("!:NO SLAVE FOUND\n");
    }
}

/**
 * @fn device map iterator
 * @return device address
 */
uint8_t dev_map_iterator() {
    static uint8_t xx = 0;
    static uint8_t yy = 0;
    static bool start = true;
    bool exist = false;
    uint8_t dev_addr;
    uint8_t test;
    
    do {
        if (xx > 7) {
            xx = 0;
            ++yy;
        }
        if (yy >= MAX_Y) {
            xx = 0;
            yy = 0;
            break;
        }
        test = (MASK << xx) & dev_map[yy];
        if (test) {
            dev_addr = yy * 8 + xx;
            exist = true;
        }
        ++xx;
    } while (!exist);
    
    if (exist) {
        exist = false;
        return dev_addr;
    } else {
        return 0;
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

    #define AQM1602XA_RN_GBW_I2C 0x10  // Character LCD
    #define A1324LUA_T_I2C 0x11  // Hall sensor
    #define HDC1000_I2C 0x12  // Temperature and humidity sensor
    #define KXR94_2050_I2C 0x13  // 3-axis accelerometer 

uint8_t schedule[20][2] = {
    {S_PLG, 0},               // 0
    {S_ADT, 0},               // 1
    {S_CMD, 0},               // 2
    {S_INV, HDC1000_I2C},     // 3
    {S_SEN, 0},                // 4
    /* {S_INV, KXR94_2050_I2C},  // 5 */
    {S_INV, 0},               // 5
    {S_SEN, 0},               // 6
    /* {S_INV, KXR94_2050_I2C},  // 7 */
    {S_INV, 0},               // 7
    {S_SEN, 0},               // 8
    {S_INV, 0},               // 9
    {S_SEN, 0},               // 10
    {S_CMD, 0},               // 11
    {S_INV, 0},               // 12
    {S_SEN, HDC1000_I2C},     // 13
    {S_INV, 0},               // 14
    /* {S_SEN, KXR94_2050_I2C},  // 15 */
    {S_SEN, 0},               // 15
    {S_INV, 0},               // 16
    /* {S_SEN, KXR94_2050_I2C},  // 17 */
    {S_SEN, 0},               // 17
    {S_INV, 0},               // 18
    {S_SEN, 0}                // 19
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
            if (dev_addr) i2c1_write_no_data(dev_addr, INV_I2C);
            break;
        case S_SEN:
            dev_addr = schedule[position][1];
            if (dev_addr) {
                status = sen(dev_addr);
                // if (status > 0) del_dev(dev_addr);
                if (status > 0) {
                    printf("!:%%%d:NETWORK_ERROR\n", dev_addr);
                    i2c1_write_no_data(dev_addr, RST_I2C);
                }   
            }
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
    } else {
        put_cmd(buf);
    }
}

void exec_remote_cmd(uint8_t idx) {
    uint8_t data;
    uint8_t type;
    uint8_t length;
    uint8_t i, status;

    /***** For debug commands *****/
    static uint8_t dev_addr;
    static uint8_t reg_addr;

    char *buf = cmd_buf[idx];
    
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
    /***** Debug commands *****/
    } else if (!strncmp(DEV, buf, 3)) {
        dev_addr = atoi(&buf[4]);
    } else if (!strncmp(REG, buf, 3)) {
        reg_addr = atoi(&buf[4]);
    } else if (!strncmp(RED, buf, 3)) {
        i2c1_read(dev_addr, reg_addr, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(WRT, buf, 3)) {
        data = atoi(&buf[4]);
        i2c1_write_no_data(dev_addr, reg_addr);
        i2c1_write_no_data(dev_addr, data);
    } else if (!strncmp(SEN, buf, 3)) {
        if (sen(BACKPLANE_SLAVE_ADDRESS) > 0) {
            printf("!:SEN:DATA NOT READY\n");
        }
    /* Extended commands */
    } else {
        length = 0;
        do {
        } while (buf[length++] != '\0');
        // printf("Extended command, length: %s, %d\n", buf, length);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, EXT_I2C);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, length);
        
        for (i=0; i<length; i++) {
            // printf("%c", buf[i]);
            status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, (uint8_t)buf[i]);                 
        }
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