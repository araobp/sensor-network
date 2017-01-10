#include <pic16f1825.h>

#include "mcc_generated_files/mcc.h"
#define _XTAL_FREQ 500000

void tmr0_handler(void) {
 
    //printf("TMR0 handler called\n");
    
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
    if (d >= 2 || d <= 400) {
        printf("%u\n", d);
    } else {
        printf("distance out of range\n");
    }
    CLRWDT();
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
    while (1)
    {
    }
}
