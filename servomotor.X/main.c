#include "mcc_generated_files/mcc.h"

#define _XTAL_FREQ 1000000

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    EPWM1_Initialize();

    /* CCPR value
      1msec: 61
      1.5msec: 92
      2msec: 122
     */
    uint16_t dutyValue = 92;
    uint8_t c;
    
    EPWM1_LoadDutyValue(dutyValue);
    while (1)
    {
        __delay_ms(500);
        LATCbits.LATC4 ^= 1;
        //printf("Hello World\n");
        if (EUSART_DataReady) {
            c = EUSART_Read();
            switch(c) {
                case 'r':
                    dutyValue = 40;
                    printf("left\n");
                    break;
                case 'm':
                    dutyValue = 100;
                    printf("middle\n");
                    break;
                case 'l':
                    dutyValue = 160;
                    printf("right\n");
                    break;
            }
            EPWM1_LoadDutyValue(dutyValue);
        }
        CLRWDT();
    } 
}
