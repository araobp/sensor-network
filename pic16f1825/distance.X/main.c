#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "HC-SR04"

uint8_t c;
uint8_t cnt;
uint8_t buf[4];
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;

uint8_t running = 1;

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

void set_handler(uint8_t value) {
    period_10 = value/10;
}

// called at every 98.304msec
void tmr0_handler(void) {
    
    if (++timer_cnt >= period_10) {
        timer_cnt = 0;

        // HC-SR04 Trigger pin
        LATCbits.LATC2 = 1;
        __delay_us(10);
        LATCbits.LATC2 = 0;

        if (running) {
            // blink LED
            LATCbits.LATC3 ^= 1;

            // HC-SR04 Echo pin
            while(!PORTCbits.RC1);
            TMR1_StartTimer();
            while(PORTCbits.RC1);
            TMR1_StopTimer();

            // TMR1 500kHz(every 2 micro second)
            uint16_t count = TMR1_ReadTimer();
            TMR1_WriteTimer(0);
            uint16_t d = (uint16_t)(count/58.82*2.0);
            if (d >= 2 && d <= 400)  {
                printf("%u\n", d);
            } else {
                printf("-1\n");  // out of range
            }
        }
    }
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
   
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    TMR1_Initialize();
    
    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Loop();
}
