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
    uint8_t measure[4];  // temperature MSB/LSB, humidity MSB/LSB
    int8_t data[2];
    int32_t t;
    uint8_t status;
    
    LATCbits.LATC7 ^= 1;

    status = i2c2_write_no_data(HDC1000_ADDR, TEMPERATURE);
    //while(LATCbits.LATC6 == 1);  // RDY pin is high
    __delay_ms(10);
    status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[0], 2);  // MSB, LSB
    __delay_ms(5);
    status = i2c2_write_no_data(HDC1000_ADDR, HUMIDITY);
    //while(LATCbits.LATC6 == 1);  // RDY pin is high
    __delay_ms(10);
    status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[2], 2);  // MSB, LSB 

    t = (int32_t)((uint16_t)measure[0] * 256 + (uint16_t)measure[1]);
    data[0] = (int8_t)(t * 165 / 65536 - 40);  // temperature

    t = (int32_t)((uint16_t)measure[2] * 256 + (uint16_t)measure[3]);
    data[1] = (int8_t)(t * 100 /65536);  // humidity

    PROTOCOL_I2C_Send_int8_t(2, data);
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{    
    //SYSTEM_Initialize();
    SSP1CON2bits.GCEN = 0;  // Disable I2C General Call
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL);
    PROTOCOL_Set_Inv_Handler(inv_handler, 20);

    EUSART_Initialize();
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // Slave initialization
    I2C2_Initialize();
    init();

    SSP1CON2bits.GCEN = 1;  // Enable I2C General Call
    I2C1_Initialize();  // Enable I2C backplane
    
    __delay_ms(2000);  // Wait for GCEN to become zero
    
    if (SSP1CON2bits.GCEN == 0) {  // I2C backplane mode
        PROTOCOL_Set_Mode(false);
    }
    
    PROTOCOL_Loop();
}