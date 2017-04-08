#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "BACKPLANE_MASTER"

#define TEST_ADDRESS 0x01
#define TEST_REGISTER 0x02
#define TEST_DATA 0x03

#define DEV "DEV"
#define REG "REG"
#define RED "RED"
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
    uint8_t dev_addr;
    uint8_t reg_addr;
    uint8_t value;
    if (!strncmp(DEV, buf, 3)) {
        dev_addr = atoi(&buf[4]);
    } else if (!strncmp(REG, buf, 3)) {
        reg_addr = atoi(&buf[4]);
    } else if (!strncmp(RED, buf, 3)) {
        i2c_read(dev_addr, reg_addr, &value, 1);
        printf("%d\n", value);
    } else if (!strncmp(WRT, buf, 3)) {
        value = atoi(&buf[4]);
        i2c_write(dev_addr, reg_addr, value);
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