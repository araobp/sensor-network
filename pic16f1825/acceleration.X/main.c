#include <pic16f1825.h>
#include "mcc_generated_files/mcc.h"
#define _XTAL_FREQ 500000
#define OFFSET 512.0
#define G 204.8  // 1024/5.0 = 204.8 (steps/volt)
#define PERIOD 1

/*
 * In case of Vdd = 5V, acceleration = measured voltage (1V = 1g).
 */
float conv(adc_result_t steps) {
    return (steps - OFFSET)/G;
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    ADC_Initialize();
    uint16_t cnt;
    float x, y, z;
    
    while (1) {
        cnt = 0;
        x = 0;
        y = 0;
        z = 0;
        
        while(cnt++ < PERIOD) {

            // X-axis
            ADC_SelectChannel(channel_AN3);
            ADC_StartConversion();
            while(!ADC_IsConversionDone());
            adc_result_t steps_x = ADC_GetConversionResult();
            x += conv(steps_x);

            // Y-axis
            ADC_SelectChannel(channel_AN2);
            ADC_StartConversion();
            while(!ADC_IsConversionDone());
            adc_result_t steps_y = ADC_GetConversionResult();
            y += conv(steps_y);

            // Z-axis
            ADC_SelectChannel(channel_AN6);
            ADC_StartConversion();
            while(!ADC_IsConversionDone());
            adc_result_t steps_z = ADC_GetConversionResult();
            z += conv(steps_z);
        }
        
        LATCbits.LATC3 ^= 1;
        printf("%.2f,%.2f,%.2f\n", x/PERIOD, y/PERIOD, z/PERIOD);
    }
}