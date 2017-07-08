#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>

#define DEVICE_ID "A1324LUA_T"

#define THRESHOLD 20
#define MIDDLE 511
#define CONTINUITY 3

bool on = false;
uint16_t sum = 0;
uint16_t count = 0;
uint8_t threshold = THRESHOLD;
uint8_t continuity = CONTINUITY;

void inv_handler(void) {
    PROTOCOL_Send_uint16_t(sum);
    sum = 0;
}
    
void loop_func(void) {
    ADC_SelectChannel(channel_AN8);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());
    adc_result_t out = ADC_GetConversionResult();
    uint16_t v = abs(out - MIDDLE);
    
    /*
     *  Count condition
     *  "^": satisfied
     *  "V": unsatisfied
     * 
     *                                   +--> sum++ and blink LED
     *                                  |
     *  on              ^ ^ ^ ^ V ^ V V V     
     * 
     *  off   ^ V ^ ^ ^                   V V V ...
     * 
     *  count 1 0 1 2 3 0 0 0 0 1 0 1 2 3 0 0 0 ...
     * 
     *       -------------------------------------> Time
     */
    switch (on) {
        case false:
            
            if (v >= THRESHOLD) count++;
            else count = 0;
            
            if (count >= CONTINUITY) {
                count = 0;
                on = true;
            }
            
            break;

        case true:
            
            if (v < THRESHOLD) count++;
            else count = 0;
            
            if (count >= CONTINUITY) {
                count = 0;
                on = false;
            LATCbits.LATC7 = 0;
            __delay_ms(3);
            LATCbits.LATC7 = 1;
            sum++;
            }
            
            break;
    }
    // printf("out:%d, v:%d, on:%d, sum:%d\n", out, v, on, sum);
}

void main(void)
{
    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, inv_handler, 125);
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
