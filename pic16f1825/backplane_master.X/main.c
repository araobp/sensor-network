#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "protocol_i2c_definition.h"
#include "i2c_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "BACKPLANE_MASTER"

#define TEST_ADDRESS 0x01
#define TEST_REGISTER 0x02
#define TEST_DATA 0x03

#define I2C "I2C"
#define DEV "DEV"
#define REG "REG"
#define RED "RED"
#define STS "STS"
#define SEN "SEN"
#define WRT "WRT"

uint8_t running = 1;
uint8_t do_func = 0;
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

void set_handler(uint8_t value) {
    period_10 = value/10;
}

void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = 1;
     }
 }

void loop_func(void) {
    if (do_func) {
        uint8_t data;
        LATCbits.LATC3 ^= 1;
        do_func = 0;
    }
}

void extension_handler(uint8_t *buf) {
    static uint8_t dev_addr;
    static uint8_t reg_addr;
    uint8_t data;
    uint8_t read_buf[16];
    uint8_t status;
    if (!strncmp(I2C, buf, 3)) {
        SLAVE_ADDRESS = atoi(&buf[4]);
    } else if (!strncmp(WHO, buf, 3)) {
        status = i2c_read(SLAVE_ADDRESS, WHO_I2C, &data, 1);
        if (status == 0) printf("%d\n", data);
        else printf("!\n");
    } else if (!strncmp(SAV, buf, 3)) {
        status = i2c_write_no_data(SLAVE_ADDRESS, SAV_I2C);
    } else if (!strncmp(STA, buf, 3)) {
        status = i2c_write_no_data(SLAVE_ADDRESS, STA_I2C);
    } else if (!strncmp(STP, buf, 3)) {
        status = i2c_write_no_data(SLAVE_ADDRESS, STP_I2C);
        if (status == 0) printf("ACK\n");
        else printf("NACK\n");
    } else if (!strncmp(SET, buf, 3)) {
        data = atoi(&buf[4]);
        i2c_write(SLAVE_ADDRESS, SET_I2C, data);
    } else if (!strncmp(GET, buf, 3)) {
        i2c_read(SLAVE_ADDRESS, GET_I2C, &data, 1);
        printf("%d\n", data);
    } else if (!strncmp(STS, buf, 3)) {
        i2c_read(SLAVE_ADDRESS, STS_I2C, &data, 1);
        printf("%d\n", data);
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
            for (int i=0; i<data; i++) printf("value[%d]: %d\n", i, read_buf[i]);        
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