#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>

#define DEVICE_ID "MULTI_A1324LUA_T"

#define THRESHOLD 50
#define MIDDLE 511
#define CONTINUITY 3
#define SLEEP_IN_LOOP 50  // in micro sec

uint8_t threshold = THRESHOLD;
uint8_t continuity = CONTINUITY;

/*
 * Six analog in
 */
adc_channel_t analog_in[6] = {
    channel_AN4, channel_AN5, channel_AN6,
    channel_AN7, channel_AN8, channel_AN11
};

/*
 * Transition
 * far -> close: 0 -> 1
 * close -> far: 1 -> 0
 */
uint8_t position[6] = {0, 0, 0, 0, 0, 0};

// Continuity counting for each analog in
uint8_t count[6] = {0, 0, 0, 0, 0, 0};

/*
 * The number of Analog-in: 1 ~ 6
 */
uint8_t ain = 1;

void led_blink(void) {
    LED_RED = 0;
    __delay_ms(3);
    LED_RED = 1;    
}

void set_handler(uint8_t value) {
    if (value <= 6) ain = value;
}

void loop_func(void) {
    uint8_t i;
    bool transition_detected = false;
    for (i=0; i < ain; i++) {
        ADC_SelectChannel(analog_in[i]);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t out = ADC_GetConversionResult();
        uint16_t v = abs(out - MIDDLE);    

        /*
         *  Count condition
         *  "^": satisfied
         *  "V": unsatisfied
         * 
         *  on              ^ ^ ^ ^ V ^ V V V     
         * 
         *  off   ^ V ^ ^ ^                   V V V ...
         * 
         *  count 1 0 1 2 3 0 0 0 0 1 0 1 2 3 0 0 0 ...
         * 
         *       -------------------------------------> Time
         */
        switch (position[i]) {
            case 0:
                if (v >= THRESHOLD) count[i]++;
                else count[i] = 0;

                if (count[i] >= CONTINUITY) {
                    count[i] = 0;
                    position[i] = 1;
                    led_blink();
                    transition_detected = true;
                }
                break;

            case 1:
                if (v < THRESHOLD) count[i]++;
                else count[i] = 0;

                if (count[i] >= CONTINUITY) {
                    count[i] = 0;
                    position[i] = 0;
                    led_blink();
                    transition_detected = true;
                }
                break;
        }
    }
    if (transition_detected) PROTOCOL_I2C_Send_uint8_t(6, position);
    __delay_us(SLEEP_IN_LOOP);
}

void main(void)
{
    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, set_handler, 1);
    PROTOCOL_Set_Func(loop_func);
        
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    ADC_Initialize();
    TMR0_Initialize();

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
