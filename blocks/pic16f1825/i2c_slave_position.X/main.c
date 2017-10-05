#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>

#define DEVICE_ID "MULTI_A1324LUA_T"

#define THRESHOLD 50
#define MIDDLE 511
#define CONTINUITY 250
#define SLEEP_IN_LOOP 1000  // micro sec

#define AINS 4
#define MASK 0b00000001

#define LED_RED_RC2 LATCbits.LATC2

uint8_t threshold = THRESHOLD;
uint8_t continuity = CONTINUITY;

/*
 * Six analog in
 */
const adc_channel_t ANALOG_IN[AINS] = {
    channel_AN2, channel_AN1, channel_AN0,
    channel_AN7
};

/*
 * SET:<value>'s value is a bit mask in deciaml.
 * 
 * SET:1 --> analog_in_flags[0] = 1;
 * SET:2 --> analog_in_flags[1] = 1;
 * SET:4 --> analog_in_flags[2] = 1;
 * SET:8 --> analog_in_flags[3] = 1;
 * 
 * SET:3 corresponds to "SET:1 and SET:2".
 */
uint8_t analog_in_flags[AINS];

/*
 * Transition
 * far -> close: 0 -> 1
 * close -> far: 1 -> 0
 */
uint8_t position[AINS];

// Continuity counting for each analog in
uint8_t count[AINS];

void led_blink(void) {
    LED_RED_RC2 = 0;
    __delay_ms(3);
    LED_RED_RC2 = 1;    
}

void init() {
    uint8_t i;
    for(i=0; i<AINS; i++) {
        position[i] = 0;
        count[i] = 0;
        analog_in_flags[i] = 0;
    }
}

void set_handler(uint8_t value) {
    uint8_t i;
    uint8_t mask = MASK;
    for(i=0; i<AINS; i++) {
        if ((value & mask) == 0) {
            analog_in_flags[i] = 0;
            position[i] = 0;
        } else {
            analog_in_flags[i] = 1;
        }
        mask = mask << 1;
    }
}

void loop_func(void) {
    uint8_t i;
    bool transition_detected = false;
    for (i=0; i < AINS; i++) {
        if (analog_in_flags[i] == 0) continue;
        
        ADC_SelectChannel(ANALOG_IN[i]);
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
            case 0:  // far
                if (v >= THRESHOLD) count[i]++;
                else count[i] = 0;

                if (count[i] >= CONTINUITY) {
                    count[i] = 0;
                    position[i] = 1;
                    led_blink();
                    transition_detected = true;
                }
                break;

            case 1:  // close
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
    if (transition_detected) {
        PROTOCOL_I2C_Send_uint8_t(AINS, position);
    }
    __delay_us(SLEEP_IN_LOOP);
}

void main(void)
{
    init();
    
    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, set_handler, NULL, 1);
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
    I2C_Initialize();  // Enable I2C backplane
    
    // Infinite loop
    PROTOCOL_Loop();
}
