#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>

#define _XTAL_FREQ 500000

#define DEVICE_ID "KXR94-2050"

#define OFFSET 512.0
#define G 204.8  // 1024/5.0 = 204.8 (steps/volt)

float get_accel(adc_channel_t ch) {
    ADC_SelectChannel(ch);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());
    adc_result_t steps = ADC_GetConversionResult();
    return (steps - OFFSET)/G;
}

float values[3];

void inv_handler(void) {
    LATCbits.LATC7 ^= 1;
    values[0] = get_accel(channel_AN3);  // x-axis
    values[1] = get_accel(channel_AN7);  // y-axis
    values[2] = get_accel(channel_AN8);  // z-axis    
    PROTOCOL_I2C_Send_float(3, values);
}

void main(void)
{    
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    ADC_Initialize();

    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, inv_handler, 10);

    // Enable interrupt
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // USART initialization
    EUSART_Initialize();
    
    // I2C backplane initialization
    I2C1_Initialize();  // Enable I2C backplane
    
    // Infinite loop
    PROTOCOL_Loop();
}
