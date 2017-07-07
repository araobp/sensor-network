#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>
#include <string.h>

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
#define HST "HST"
#define DSP "DSP"
#define CUL "CUL"
#define CUR "CUR"
#define NWL "NWL"
#define HOM "HOM"
#define CNT "CNT"
#define LED "LED"

#define ON "ON"
#define OFF "OFF"

char *pbuf = NULL;

void lcd_init(void) {
    __delay_ms(50);
    write_command(0x38);
    write_command(0x39);
    write_command(0x14);
    write_command(0x73);  // Contrast: C3=0 C2=0 C1=1 C0=1
    write_command(0x52);  // Contrast: BON=0 C5=1 C4=0
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

void lcd_string(void) {
    uint8_t i = 4;
    while (pbuf[i] != '\0') {
        write_data(pbuf[i++]);
    }
}

/*
 * String format
 * 1st line: 16 characters
 * 2nd line: 16 characters
 */
void lcd_string_2lines(void) {
    uint8_t i = 4;
    while (pbuf[i] != '\0') {
        write_data(pbuf[i++]);
        if (i == 20) write_command(0xc0); // new line
    }
}

/*
 * contrast data
 * XX XX C5 C4 C3 C2 C1 C0
 * MSB                  LSB
 * 0 ~ 63 levels
 */
void lcd_contrast(uint8_t contrast) {
    write_command(0x39);
    write_command(0x70 | (contrast & 0b00001111));  // C3 C2 C1 C0
    write_command(0x50 | ((contrast >> 4) & 0b00000111));  // BON C5 C4
    write_command(0x38);
}

void lcd_arao(void) {
    // Print my name in Japanese Katakana
    write_data(0xb1);
    write_data(0xd7);
    write_data(0xb5);        
}

void lcd_test(void) {
    write_data(0x33);  // 3
    write_data(0x37);  // 7
    write_data(0x2e);  // .
    write_data(0x30);  // 0
    write_data(0xf2);  // o
    write_data(0x43);  // C
    write_command(0xc0);  // new line
    write_data(0x28);  // (
    write_data(0x5e);  // ~
    write_data(0x2d);  // -
    write_data(0x5e);  // ~
    write_data(0x29);  // )
}

void extension_handler(char *buf) {
    uint8_t value;
    if (!PROTOCOL_Read_Lock()) {
        pbuf = buf;
        PROTOCOL_Set_Lock(true);
    }
}

void loop_func(void) {
    uint8_t value;
    if(PROTOCOL_Read_Lock()) {
        //printf("%s\n", pbuf);
        if (parse(INI, pbuf)) {
            lcd_init();
        } else if (parse(CMD, pbuf)) {
            value = atoi(&pbuf[4]);
            write_command(value);
        } else if (parse(DAT, pbuf)) {
            value = atoi(&pbuf[4]);
            write_data(value);
        } else if (parse(CLR, pbuf)) {
            lcd_clear();
        } else if (parse(STR, pbuf)) {
            lcd_string();
        } else if (parse(HST, pbuf)) {
            write_command(0x02);  // return home
            lcd_string();   
        } else if (parse(DSP, pbuf)) {
            write_command(0x02);  // return home
            lcd_string_2lines();
        } else if (parse(CUL, pbuf)) {
            write_command(0x10);
        } else if (parse(CUR, pbuf)) {
            write_command(0x14);        
        } else if (parse(NWL, pbuf)) {
            write_command(0xc0);
        } else if (parse(HOM, pbuf)) {
            write_command(0x02);
        } else if (parse(CNT, pbuf)) {
            value = atoi(&pbuf[4]);
            lcd_contrast(value);
        } else if (parse(LED, pbuf)) {
            if (!strncmp(ON, &pbuf[4], 2)) {
                LATCbits.LATC7 = 0;  // turn LED on
            } else if (!strncmp(OFF, &pbuf[4], 3)) {
                LATCbits.LATC7 = 1;  // turn LED off     
            }
        }
        pbuf = NULL;
        PROTOCOL_Set_Lock(false);
    }
}

void main(void)
{    
    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, NULL, 20);
    PROTOCOL_Set_Extension_Handler(extension_handler);
    PROTOCOL_Set_Func(loop_func);

    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    
    // Enable interrupt
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // Device initialization
    I2C2_Initialize();
    lcd_init();
    //lcd_arao();
    lcd_test();
    
    // turn LED off
    LATCbits.LATC7 = 1;
    
    // USART initialization
    EUSART_Initialize();
    
    // I2C backplane initialization
    I2C1_Initialize();  // Enable I2C backplane
    
    // Infinite loop
    PROTOCOL_Loop();
}
