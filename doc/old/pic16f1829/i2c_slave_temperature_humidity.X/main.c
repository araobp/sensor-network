#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "HDC1000"
#define HDC1000_ADDR 0x0040

#define TEMPERATURE 0x00
#define HUMIDITY 0x01
#define CONFIGURATION 0x02

#define CONFIG_DATA 0x00

bool running = true;
bool do_func = false;
uint8_t timer_cnt = 0;
uint8_t period_10;

void start_handler(void) {
    running = true;
}

void stop_handler(void) {
    running = false;
}

void set_handler(uint8_t value) {
    period_10 = value;
}


void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = true;
     }
}

void init(void) {
    __delay_ms(15);  // wait for 15msec
    i2c2_write(HDC1000_ADDR, CONFIGURATION, CONFIG_DATA);
}

void loop_func(void) {
    uint8_t measure[4];  // temperature MSB/LSB, humidity MSB/LSB
    int8_t data[2];
    int32_t t;

    if (do_func) {

         LATCbits.LATC7 ^= 1;
        
        i2c2_write_no_data(HDC1000_ADDR, TEMPERATURE);
        while(LATCbits.LATC6 == 1);
        i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[0], 2);  // MSB, LSB
        
        i2c2_write_no_data(HDC1000_ADDR, HUMIDITY);
        while(LATCbits.LATC6 == 1);
        i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[2], 2);  // MSB, LSB
        
        t = (int32_t)((uint16_t)measure[0] * 256 + (uint16_t)measure[1]);
        data[0] = (int8_t)(t * 165 / 65536 - 40);  // temperature

        t = (int32_t)((uint16_t)measure[2] * 256 + (uint16_t)measure[3]);
        data[1] = (int8_t)(t * 100 /65536);  // humidity
            
        PROTOCOL_I2C_Send_int8_t(2, data);
        
        do_func = false;
    }
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{    
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    
    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Set_Func(loop_func);

    EUSART_Initialize();
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    I2C2_Initialize();
    I2C1_Initialize();
       
    init();
    
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Loop();
}