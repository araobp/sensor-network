#include <pic16f1825.h>
#include "mcc_generated_files/mcc.h"
#define _XTAL_FREQ 500000

#define RANGE 2.0

float calc_g(adc_result_t result) {
    return RANGE * ((float)result - 512.0) / 512.0;
}

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
        float x_g = calc_g(x);
       
        // Y-axis
        ADC_SelectChannel(channel_AN2);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t y = ADC_GetConversionResult();
        float y_g = calc_g(y);
       
        __delay_ms(100);
        // Z-axis
        ADC_SelectChannel(channel_AN6);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t z = ADC_GetConversionResult();
        float z_g = calc_g(z);
        
        
        //printf("%d\n", x);
        printf("%f,%f,%f\n", x_g, y_g, z_g);

    }
}