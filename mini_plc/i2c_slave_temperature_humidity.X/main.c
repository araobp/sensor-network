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

bool invoked = false;

void init(void) {
    __delay_ms(30);  // wait for 15msec
    i2c2_write(HDC1000_ADDR, CONFIGURATION, CONFIG_DATA);
}

void inv_handler(void) {
    LATCbits.LATC7 ^= 1;
    invoked = true;
}

void loop_func(void) {
    uint8_t measure[4];  // temperature MSB/LSB, humidity MSB/LSB
    int8_t data[2];
    int32_t t;
    uint8_t status;

    
    data[0] = 11;
    data[1] = 22;

    if (invoked) {
        PROTOCOL_I2C_Send_int8_t(2, data);
        invoked = false;
    }
    
//    __delay_ms(1000);
//    invoked = true;
    /**
    if (invoked) {

         LATCbits.LATC7 ^= 1;

        status = i2c2_write_no_data(HDC1000_ADDR, TEMPERATURE);
        printf("--\n%d\n", status);
        //while(LATCbits.LATC6 == 1);
        __delay_ms(10);
        status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[0], 2);  // MSB, LSB
        printf("%d\n", status);
        __delay_ms(5);
        status = i2c2_write_no_data(HDC1000_ADDR, HUMIDITY);
        printf("%d\n", status);
        //while(LATCbits.LATC6 == 1);
        __delay_ms(10);
        status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[2], 2);  // MSB, LSB 
        printf("%d\n", status);

        t = (int32_t)((uint16_t)measure[0] * 256 + (uint16_t)measure[1]);
        data[0] = (int8_t)(t * 165 / 65536 - 40);  // temperature

        t = (int32_t)((uint16_t)measure[2] * 256 + (uint16_t)measure[3]);
        data[1] = (int8_t)(t * 100 /65536);  // humidity

        PROTOCOL_I2C_Send_int8_t(2, data);
        
    }
   
    invoked = false;
//    invoked = true;
   */
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
    
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL);
    PROTOCOL_Set_Inv_Handler(inv_handler);

    EUSART_Initialize();
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    I2C2_Initialize();
    init();

    I2C1_Initialize();
           
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Loop();
}