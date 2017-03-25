#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "KXR94-2050"
#define OFFSET 512.0
#define G 204.8  // 1024/5.0 = 204.8 (steps/volt)

uint8_t running = 1;
uint8_t do_func = 0;
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;

float get_accel(adc_channel_t ch) {
    ADC_SelectChannel(ch);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());
    adc_result_t steps = ADC_GetConversionResult();
    return (steps - OFFSET)/G;
}

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

void set_handler(uint8_t value) {
    period_10 = value/10;
}

void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = 1;
     }
 }

void loop_func(void) {
    float x, y, z;
    if (do_func) {
        x = get_accel(channel_AN3);
        y = get_accel(channel_AN2);
        z = get_accel(channel_AN6);
        printf("%.2f,%.2f,%.2f\n",x,y,z);
        LATCbits.LATC3 ^= 1;
        do_func = 0;
    }
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    ADC_Initialize();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Loop();
}