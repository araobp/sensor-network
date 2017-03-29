#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c_util.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "AQM1602XA-RN-GBW"

#define LCD_ADDRESS 0x3e  // AQM1602XA-RN-GBW
#define COMMAND 0x00
#define DATA 0x40

void tmr0_handler(void) {
    LATCbits.LATC3 ^= 1;
} 

void write_command(uint8_t command) {
    i2c_write(LCD_ADDRESS, COMMAND, command);
    __delay_ms(1);
}

void write_data(uint8_t data) {
    i2c_write(LCD_ADDRESS, DATA, data);
    __delay_ms(1);
}

void extension_handler(uint8_t *buf) {
    printf("%s\n", buf);
}

void lcd_init(void) {
    
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
   __delay_ms(50);
}

void lcd_arao(void) {
    // Print my name in Japanese Katakana
    write_data(0xb1);
    write_data(0xd7);
    write_data(0xb5);        
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    I2C_Initialize();
    
    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, 0, 0, 0);
    PROTOCOL_Set_Extension_Handler(extension_handler);
    
    lcd_init();
    lcd_arao();
        
    PROTOCOL_Loop();
}