#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"

#define _XTAL_FREQ 500000

uint8_t c;
uint8_t cnt;
uint8_t buf[4];
uint8_t period;
uint8_t period_10;
uint8_t timer_cnt = 0;

// called at every 98.304msec
void tmr0_handler(void) {
    
    if (++timer_cnt >= period_10) {
        timer_cnt = 0;

        // blink LED
        LATCbits.LATC3 ^= 1;

        // HC-SR04 Trigger pin
        LATCbits.LATC2 = 1;
        __delay_us(10);
        LATCbits.LATC2 = 0;

        // HC-SR04 Echo pin
        while(!PORTCbits.RC1);
        TMR1_StartTimer();
        while(PORTCbits.RC1);
        TMR1_StopTimer();

        // TMR1 500kHz(every 2 micro second)
        uint16_t count = TMR1_ReadTimer();
        TMR1_WriteTimer(0);
        uint16_t d = (uint16_t)(count/58.82*2.0);
        if (d >= 2 || d <= 400)  {
            printf("%u\n", d);
        } else {
            printf("distance out of range\n");
        }
    }
    CLRWDT();
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    period = DATAEE_ReadByte(0);
    if (period < 10) period = 10;
    period_10 = period/10;
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    TMR1_Initialize();
    EUSART_Initialize();
    printf("Period: %x\n", period);

    while (1)
    {
        do {
            c = EUSART_Read();
            printf("%c\n", c);
            if (c == '\n') {
                
                buf[cnt] = '\0';
                cnt = 0;
                period = atoi(buf);
                period_10 = period/10;
                DATAEE_WriteByte(0, period);
                printf("New Period: %s\n", buf);
            } else if (cnt < 3) {
                buf[cnt++] = c;
            } else {
                cnt = 0;
                break;
            }
        } while (EUSART_DataReady);
    }
}
