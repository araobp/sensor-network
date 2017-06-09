#include "mcc_generated_files/mcc.h"
#include "i2c_util.h"

#define _XTAL_FREQ 500000

#define LCD_ADDRESS 0x3e  // AQM1602XA-RN-GBW
#define COMMAND 0x00
#define DATA 0x40

void write_command(uint8_t command) {
    i2c_write(LCD_ADDRESS, COMMAND, command);
    __delay_ms(1);
}

void write_data(uint8_t data) {
    i2c_write(LCD_ADDRESS, DATA, data);
    __delay_ms(1);
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    I2C_Initialize();
    EUSART_Initialize();

    // Initialization
    __delay_ms(50);
    write_command(0x38);
    write_command(0x39);
    write_command(0x14);
    write_command(0x7c);  // Contrast: C3=1 C2=1 C1=0 C0=0
    write_command(0x50);  // Contrast: C5=0 C4=0
    write_command(0x6c);
    __delay_ms(250);
    write_command(0x38);
    write_command(0x01);
    write_command(0x0c);
    
    // Print my name
    __delay_ms(50);
    write_data(0xb1);
    write_data(0xd7);
    write_data(0xb5);
    
    while (1) {
    }
}