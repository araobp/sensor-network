#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "AQM1602XA-RN-GBW"

#define _XTAL_FREQ 500000
#define DEVICE_ID "AQM1602XA-RN-GBW"


/* Here I dare use macro to reduce the program stack level
 * #define LCD_ADDRESS 0x3e  // AQM1602XA-RN-GBW
 * #define COMMAND 0x00
 * #define DATA 0x40
 */
#define write_command(command)     i2c2_write(0x3e, 0x00, command);\
                                   __delay_ms(1);
#define write_data(data)           i2c2_write(0x3e, 0x40, data);\
                                   __delay_ms(1);

#define INI "INI"
#define CMD "CMD"
#define DAT "DAT"
#define CLR "CLR"
#define STR "STR"
#define CUL "CUL"
#define CUR "CUR"
#define NWL "NWL"
#define HOM "HOM"

#define PERIOD_10 100

bool running = true;
uint8_t do_func = 0;
uint8_t timer_cnt = 0;
char *pbuf = NULL;

void tmr0_handler(void) {
     if (++timer_cnt >= PERIOD_10) {
        timer_cnt = 0;
        LATCbits.LATC7 ^= 1;
     }
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

void lcd_clear(void) {
    write_command(0x01);
}

void lcd_arao(void) {
    // Print my name in Japanese Katakana
    write_data(0xb1);
    write_data(0xd7);
    write_data(0xb5);        
}

bool lock = false;

/*
void blink_led(uint8_t times) {
    uint8_t i;
    for(i=0;i<times;i++) {
        LATCbits.LATC7 = 0;
        __delay_ms(50);
        LATCbits.LATC7 = 1;   
        __delay_ms(50);
    }
}
*/

void extension_handler(char *buf) {
    uint8_t value;
    if (!PROTOCOL_Read_Lock()) {
        pbuf = buf;
        PROTOCOL_Set_Lock(true);
    }
}

void start_handler(void) {
}

void stop_handler(void) {
}

void set_handler(uint8_t value) {
}

void loop_func(void) {
    uint8_t value;
    if(PROTOCOL_Read_Lock()) {
        //printf("%s\n", pbuf);
        if (!strncmp(INI, pbuf, 3)) {
            lcd_init();
        } else if (!strncmp(CMD, pbuf, 3)) {
            value = atoi(&pbuf[4]);
            write_command(value);
        } else if (!strncmp(DAT, pbuf, 3)) {
            value = atoi(&pbuf[4]);
            write_data(value);
        } else if (!strncmp(CLR, pbuf, 3)) {
            lcd_clear();
        } else if (!strncmp(STR, pbuf, 3)) {
            uint8_t i = 4;
            while (pbuf[i] != '\0') {
                write_data(pbuf[i++]);
            }
        } else if (!strncmp(CUL, pbuf, 3)) {
            write_command(0x10);
        } else if (!strncmp(CUR, pbuf, 3)) {
            write_command(0x14);        
        } else if (!strncmp(NWL, pbuf, 3)) {
            write_command(0xC0);
        } else if (!strncmp(HOM, pbuf, 3)) {
            write_command(0x02);
        }
        pbuf = NULL;
        PROTOCOL_Set_Lock(false);
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
    PROTOCOL_Set_Extension_Handler(extension_handler);
    PROTOCOL_Set_Func(loop_func);

    EUSART_Initialize();
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    I2C2_Initialize();
    I2C1_Initialize();
       
    lcd_init();
    lcd_arao();
        
    PROTOCOL_Loop();
}