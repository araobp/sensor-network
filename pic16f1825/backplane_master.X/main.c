#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "BACKPLANE_MASTER"

//#define TEST_ADDRESS 0x01
//#define TEST_REGISTER 0x02
//#define TEST_DATA 0x03

#define I2C "I2C"
#define MAP "MAP"
#define DEV "DEV"
#define REG "REG"
#define RED "RED"
#define STS "STS"
#define SEN "SEN"
#define WRT "WRT"

#define MAX_DEV_ADDR 48  // it must be larger than 16 and multipels of 8

bool running = true;
bool do_func_plg = false;
bool do_func_sts = false;
uint8_t timer_cnt_plg = 0;
uint8_t timer_cnt_sts = 0;
uint8_t t_sts = T_STS;

void start_handler(void) {
    running = true;
}

void stop_handler(void) {
    running = false;
}

void set_handler(uint8_t value) {
    t_sts = value/10;
}

void tmr0_handler(void) {
     if (++timer_cnt_plg >= T_PLG) {
        timer_cnt_plg = 0;
        do_func_plg = true;
     }
     if (++timer_cnt_sts >= t_sts) {
        timer_cnt_sts = 0;
        do_func_sts = true;
     }
 }

uint8_t dev_map[MAX_DEV_ADDR/8];  // I2C slave device map
uint8_t x, y;

void clear_dev_map(void) {
    for (y=0; y<MAX_DEV_ADDR/8; y++) {
        dev_map[y] = 0;
    }
}

void add_dev(uint8_t dev_addr) {
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        dev_map[y] = dev_map[y] | (0x01 << x);
    }
}

void del_dev(uint8_t dev_addr) {
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        dev_map[y] = dev_map[y] & ~(0x01 << x);
    }
}

void print_dev_map(void) {
    uint8_t yy = MAX_DEV_ADDR/8 - 1;
    for (y=0; y<yy; y++) printf("%x,", dev_map[y]);
    printf("%x\n", dev_map[y]);
}

uint8_t dev_map_iterator() {
    static uint8_t x = 0;
    static uint8_t y = 0;
    static const uint8_t yy = MAX_DEV_ADDR/8 - 1;
    static bool start = true;
    static const uint8_t mask = 0b00000001;
    bool exist = false;
    uint8_t x_loc;
    uint8_t dev_addr;
    do {
        if (x > 7) {
            x = 0;
            ++y;
        }
        if (y >= yy) {
            x = 0;
            y = 0;
            break;
        }
        x_loc = mask << x;
        if (x_loc & dev_map[y]) {
            dev_addr = y * 8 + x;
            exist = true;
        }
        ++x;
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
    uint8_t read_buf[16];
    uint8_t length ,data, i;
    status = i2c_read(BACKPLANE_SLAVE_ADDRESS, STS_I2C, &data, 1);
    if (data == STS_SEN_READY) {
        status = i2c_read(BACKPLANE_SLAVE_ADDRESS, SEN_I2C, &type, 1);
        if (status == 0) {
            status = i2c_read(BACKPLANE_SLAVE_ADDRESS, SEN_I2C, &length, 1);
            if (status == 0) {
                status = i2c_read(BACKPLANE_SLAVE_ADDRESS, SEN_I2C, &read_buf[0], length);
                if (status == 0) {
                    printf("%%%d:%d,%d,", dev_addr, type, length);
                    if (length > 1) for (i=0; i < length - 1; i++) printf("%d,", read_buf[i]);
                    printf("%d\n", read_buf[i]);
                }
            }                    
        }   
    } else {
        printf("!:%%%d:DATA NOT READY\n", dev_addr);
    }
    return status;
}

void loop_func(void) {
    uint8_t dev_addr;
    uint8_t read_buf[16];
    bool blink_led = false;
    if (do_func_plg) {
        //printf("general call\n");
        uint8_t status = i2c_write_no_data(GENERAL_CALL_ADDRESS, PLG_I2C);
        //printf("%d\n", status);
        if (status == 0) {
            for (dev_addr=1; dev_addr<=MAX_DEV_ADDR; dev_addr++) {
                status = i2c_read(dev_addr, WHO_I2C, &read_buf[0], 1);
                if (status == 0) {
                    //printf("%d\n", read_buf[0]);
                    add_dev(read_buf[0]);
                    //print_dev_map();
                }
            }
        }
        do_func_plg = false;
        blink_led = true;
    }
    if (do_func_sts) {
        dev_addr = 0;
        for (dev_addr = dev_map_iterator(); dev_addr > 0 ; dev_addr = dev_map_iterator()) {
            uint8_t status = sen(dev_addr);
            if (status > 0) del_dev(dev_addr);
        }
    }
    if (blink_led) {
        LATCbits.LATC3 ^= 1;
        blink_led = false;
    }
}

void extension_handler(uint8_t *buf) {
    static uint8_t dev_addr;
    static uint8_t reg_addr;
    uint8_t data;
    uint8_t type;
    uint8_t length;
    uint8_t read_buf[16];
    uint8_t i, status;
        
    if (!strncmp(I2C, buf, 3)) {
        BACKPLANE_SLAVE_ADDRESS = atoi(&buf[4]);
    } else if (!strncmp(WHO, buf, 3)) {
        status = i2c_read(BACKPLANE_SLAVE_ADDRESS, WHO_I2C, &data, 1);
        if (status == 0) printf("%d\n", data);
        else printf("!\n");
    } else if (!strncmp(MAP, buf, 3)) {
        print_dev_map();
    } else if (!strncmp(SAV, buf, 3)) {
        status = i2c_write_no_data(BACKPLANE_SLAVE_ADDRESS, SAV_I2C);
    } else if (!strncmp(STA, buf, 3)) {
        status = i2c_write_no_data(BACKPLANE_SLAVE_ADDRESS, STA_I2C);
    } else if (!strncmp(STP, buf, 3)) {
        status = i2c_write_no_data(BACKPLANE_SLAVE_ADDRESS, STP_I2C);
        if (status == 0) printf("ACK\n");
        else printf("NACK\n");
    } else if (!strncmp(SET, buf, 3)) {
        data = atoi(&buf[4]);
        i2c_write(BACKPLANE_SLAVE_ADDRESS, SET_I2C, data);
    } else if (!strncmp(GET, buf, 3)) {
        i2c_read(BACKPLANE_SLAVE_ADDRESS, GET_I2C, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(STS, buf, 3)) {
        i2c_read(BACKPLANE_SLAVE_ADDRESS, STS_I2C, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(SEN, buf, 3)) {
        sen(BACKPLANE_SLAVE_ADDRESS);
    } else if (!strncmp(DEV, buf, 3)) {
        dev_addr = atoi(&buf[4]);
    } else if (!strncmp(REG, buf, 3)) {
        reg_addr = atoi(&buf[4]);
    } else if (!strncmp(RED, buf, 3)) {
        i2c_read(dev_addr, reg_addr, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(SEN, buf, 3)) {
        status = i2c_read(dev_addr, SEN_I2C, &data, 1);
        if (status == 0) printf("type: %d\n", data);
        else printf("status: %d\n", status);
        status = i2c_read(dev_addr, SEN_I2C, &data, 1);
        if (status == 0) printf("length: %d\n", data);
        else printf("status: %d\n", status);
        status = i2c_read(dev_addr, SEN_I2C, &read_buf[0], data);
        if (status == 0) {
            for (i=0; i<data; i++) printf("value[%d]: %d\n", i, read_buf[i]);        
        } else {
            printf("status: %d\n", status);
        }
    } else if (!strncmp(WRT, buf, 3)) {
        data = atoi(&buf[4]);
        i2c_write(dev_addr, reg_addr, data);
    }
}

void main(void)
{
    clear_dev_map();

    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    I2C_Initialize();
    
    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Set_Extension_Handler(extension_handler);
    PROTOCOL_Loop();
}