#include <pic16f1825.h>
#include "mcc_generated_files/mcc.h"
#define _XTAL_FREQ 500000

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    ADC_Initialize();
    while (1)
    {
        __delay_ms(1000);
        LATCbits.LATC3 ^= 1;

        
        // X-axis
        ADC_SelectChannel(channel_AN3);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t x = ADC_GetConversionResult();
       
        // Y-axis
        ADC_SelectChannel(channel_AN2);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t y = ADC_GetConversionResult();
       
        // Z-axis
        ADC_SelectChannel(channel_AN6);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t z = ADC_GetConversionResult();        
        
        printf("%d,%d,%d\n", x, y, z);
    }
}