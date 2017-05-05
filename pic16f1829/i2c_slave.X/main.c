#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "TEMPLATE_PIC16F1829"

#define AAA "AAA"
#define BBB "BBB"

bool running = true;
uint8_t do_func = 0;
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;
char *ext_buf = NULL;
uint8_t blink_times = 0;

void start_handler(void) {
    running = true;
    //printf("Start Handler called\n");
}

void stop_handler(void) {
    running = false;
    //printf("Stop Handler called\n");
}

void set_handler(uint8_t value) {
    period_10 = value;
    //printf("Set Handler called\n");
}

void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = 1;
     }
 }

void blink(uint8_t times) {
    uint8_t i;
    for(i=0;i<times;i++) {
        LATCbits.LATC7 = 0;
        __delay_ms(50);
        LATCbits.LATC7 = 1;   
        __delay_ms(50);
    }
}

/*
 * Note: this function must complete its task in a short time.
 */
void extension_handler(char *char_buf) {
    uint8_t value;
    ext_buf = char_buf;
    if (!strncmp(AAA, char_buf, 3)) {
        blink_times = 1;
    } else if (!strncmp(BBB, char_buf, 3)) {
        value = atoi(&char_buf[4]);
        blink_times = value;
    }
}

void loop_func(void) {
    uint8_t pbuf[6];
    uint8_t pbuf_uint8_t[5] = {0, 1, 2, 3, 255};
    int8_t pbuf_int8_t[5] = {-128, -10, 0, 10, 127};
    uint16_t pbuf_uint16_t[5] = {0xffff, 0x0f00, 0x0100, 0x00ff, 0x0001};
    int16_t pbuf_int16_t[5] = {-32768, -1000, 0, 1000, 32767};
    float pbuf_float[7] = {-327.68, -1.99, -1.01, 0.00, 1.01, 1.99, 327.67};
    static uint8_t turn = TYPE_UINT8_T;
    int16_t float100;
    if (ext_buf != NULL) {
        printf("%s\n", ext_buf);
        ext_buf = NULL;
    }
    if (blink_times > 0) {
        blink(blink_times);
        blink_times = 0;
    }
    if (do_func) {
        LATCbits.LATC7 ^= 1;
        switch(turn) {
            case TYPE_UINT8_T:
                PROTOCOL_I2C_Send_uint8_t(5, &pbuf_uint8_t[0]);
                turn = TYPE_INT8_T;
                break;
            case TYPE_INT8_T:
                PROTOCOL_I2C_Send_int8_t(5, &pbuf_int8_t[0]);
                turn = TYPE_UINT16_T;
                break;
            case TYPE_UINT16_T:
                PROTOCOL_I2C_Send_uint16_t(5, &pbuf_uint16_t[0]);
                turn = TYPE_INT16_T;
                break;
            case TYPE_INT16_T:
                PROTOCOL_I2C_Send_int16_t(5, &pbuf_int16_t[0]);
                turn = TYPE_FLOAT;
                break;
            case TYPE_FLOAT:
                PROTOCOL_I2C_Send_float(7, &pbuf_float[0]);
                turn = TYPE_UINT8_T;
                break;                
        }
        do_func = 0;
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

    I2C1_Initialize();

    PROTOCOL_Loop();
}