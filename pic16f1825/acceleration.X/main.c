#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "KXR94-2050"
#define OFFSET 512.0
#define G 204.8  // 1024/5.0 = 204.8 (steps/volt)

/*
 * In case of Vdd = 5V, acceleration = measured voltage (1V = 1g).
 */
int16_t get_accel(adc_channel_t ch) {
    ADC_SelectChannel(ch);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());
    adc_result_t steps = ADC_GetConversionResult();
    return (int16_t)((steps - OFFSET)/G * 1000);
}

void tmr0_handler(void) {
    int16_t x = get_accel(channel_AN3);
    int16_t y = get_accel(channel_AN2);
    int16_t z = get_accel(channel_AN6);
    LATCbits.LATC3 ^= 1;
    printf("%d,%d,%d\n", x, y, z);  // outputs milli G
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

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    PROTOCOL_Initialize(DEVICE_ID, 0, 0, 0);
    PROTOCOL_Loop();
}