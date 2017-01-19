#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#define _XTAL_FREQ 500000

#define THRESHOLD 8
#define MIDDLE 511

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    ADC_Initialize();
    uint16_t cnt = 0;
    uint16_t sum_out = 0;
    bool on = false;
    uint8_t led_cnt = 0;
    
    while (1)
    {
        __delay_ms(5);
        if (LATCbits.LATC3 == 1 && ++led_cnt >= 20) {
            LATCbits.LATC3 = 0;
        }

        // Out
        ADC_SelectChannel(channel_AN6);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t out = ADC_GetConversionResult();
        
        uint16_t v = abs(out - MIDDLE);
        if (on == false && v > THRESHOLD) {
            cnt++;
            if (cnt > 3) {
                cnt = 0;
                on = true;
            }
        } else if (on == true && v < THRESHOLD) {
            cnt++;
            if (cnt > 3) {
                cnt = 0;
                on = false;
                LATCbits.LATC3 = 1;
                printf("1\n");
            }
        }
        //printf("%d %d %d\n", v, cnt, on);
    }
}