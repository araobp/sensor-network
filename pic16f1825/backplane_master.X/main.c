#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c_util.h"
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

const uint8_t MAX_Y = MAX_DEV_ADDR/8 - 1;

bool running = true;
bool do_func_plg = false;
bool do_func_sts = false;
uint8_t timer_cnt_plg = 0;
uint8_t timer_cnt_sts = 0;
uint8_t t_sts = T_STS;
uint8_t read_buf[16];
uint8_t dev_map[MAX_DEV_ADDR/8];  // I2C slave device map
uint8_t x, y;

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
    for (y=0; y<MAX_Y; y++) printf("%x,", dev_map[y]);
    printf("%x\n", dev_map[y]);
}

uint16_t concat(uint8_t msb, uint8_t lsb) {
    return (uint16_t)msb << 8 & (uint16_t)lsb;    
}

void print_tlv(uint8_t dev_addr, uint8_t type, uint8_t length, uint8_t *pbuffer) {
    uint8_t i;
    uint8_t len;
    int16_t v;
    printf("%%%d:", dev_addr);
    switch(type) {
        case TYPE_UINT8_T:
            len = length - 1;
            for (i=0; i<len; i++) printf("%d,", pbuffer[i]);
            printf("%d\n", pbuffer[i]);
            break;
        case TYPE_INT8_T:
            len = length - 1;
            for (i=0; i<len; i++) printf("%d,", (int8_t)pbuffer[i]);
            printf("%d\n", (int8_t)pbuffer[i]);            
            break;
        case TYPE_UINT16_T:
            len = length - 2;
            for (i=0; i<len; i++) printf("%d,", concat(pbuffer[i], pbuffer[++i]));
            printf("%d\n", concat(pbuffer[i], pbuffer[++i]));                        
            break;
        case TYPE_INT16_T:
            len = length - 2;
            for (i=0; i<len; i++) printf("%d,", (int16_t)(concat(pbuffer[i], pbuffer[++i])));
            printf("%d\n", (int16_t)(concat(pbuffer[i], pbuffer[++i])));                        
            break;
        case TYPE_FLOAT:
            len = length - 2;
            for (i=0; i<len; i++) {
                v = (int16_t)(concat(pbuffer[i], pbuffer[++i]));
                printf("%d.%d,", v/100, v%100);
            }
            v = (int16_t)(concat(pbuffer[i], pbuffer[++i]));
            printf("%d.%d,", v/100, v%100);
            break;
    }
}

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
    status = i2c_read(dev_addr, STS_I2C, &data, 1);
    if (data == STS_SEN_READY) {
        status = i2c_read(dev_addr, SEN_I2C, &type, 1);
        if (status == 0) {
            status = i2c_read(dev_addr, SEN_I2C, &length, 1);
            if (status == 0) {
                status = i2c_read(dev_addr, SEN_I2C, &read_buf[0], length);
                if (status == 0) {
                    print_tlv(dev_addr, type, length, &read_buf[0]);
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
    bool blink_led = false;
    if (do_func_plg) {
        uint8_t status = i2c_write_no_data(GENERAL_CALL_ADDRESS, PLG_I2C);
        if (status == 0) {
            for (dev_addr=1; dev_addr<=MAX_DEV_ADDR; dev_addr++) {
                status = i2c_read(dev_addr, WHO_I2C, &read_buf[0], 1);
                if (status == 0) {
                    add_dev(read_buf[0]);
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
    uint8_t data;
    uint8_t type;
    uint8_t length;
    uint8_t i, status;

    /***** For debug commands *****/
    static uint8_t dev_addr;
    static uint8_t reg_addr;

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
    
    /***** Debug commands *****/
    } else if (!strncmp(DEV, buf, 3)) {
        dev_addr = atoi(&buf[4]);
    } else if (!strncmp(REG, buf, 3)) {
        reg_addr = atoi(&buf[4]);
    } else if (!strncmp(RED, buf, 3)) {
        i2c_read(dev_addr, reg_addr, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(WRT, buf, 3)) {
        data = atoi(&buf[4]);
        i2c_write(dev_addr, reg_addr, data);
    } else if (!strncmp(SEN, buf, 3)) {
        sen(BACKPLANE_SLAVE_ADDRESS);
    /******/
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

    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, set_handler);
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Set_Extension_Handler(extension_handler);
    PROTOCOL_Loop();
}