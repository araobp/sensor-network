#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart.h"

#define _XTAL_FREQ 500000

// L3GD20 gyroscope
#define L3GD20_ADDRESS 0b1101011
#define WHO_AM_I 0x0f
#define WHO_AM_I_RESPONSE 0xd4
#define CTRL_REG1 0x20;
#define OUT_TEMP 0x26;
const byte OUT_X_L = 0x28;
const byte OUT_X_H = 0x29;
const byte OUT_Y_L = 0x2a;
const byte OUT_Y_H = 0x2b;
const byte OUT_Z_L = 0x2c;
const byte OUT_Z_H = 0x2d;

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    I2C1_Initialize();
    EUSART_Initialize();
    
    uint8_t buf[2];
    I2C1_MESSAGE_STATUS status;

    printf("Init completed\n");
    
    buf[0] = WHO_AM_I;
    
    I2C1_MasterWrite(buf, 1, L3GD20_ADDRESS, &status);
    while (status == I2C1_MESSAGE_PENDING);
    if (status == I2C1_MESSAGE_COMPLETE) {
        I2C1_MasterRead(buf, 1, L3GD20_ADDRESS, &status); 
        while (status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE) {
            if (buf[0] == WHO_AM_I_RESPONSE) {
                printf("WHO_AM_I: OK\n");
            } else {
                printf("WHO_AM_I: NG\n");
            }
        }
    }
    
    while (1)
    {
        __delay_ms(1000);
        printf("Hello World!\n");
        LATBbits.LATB7 ^= 1;
        CLRWDT();
    }
}