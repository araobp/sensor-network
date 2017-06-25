#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>

#define _XTAL_FREQ 500000

#define DEVICE_ID "A1324LUA_T"

#define THRESHOLD 8
#define MIDDLE 511
#define CONTINUITY 3
#define INTERVAL 10

bool on = false;
uint16_t sum = 0;
uint16_t count = 0;
uint8_t threshold = THRESHOLD;
uint8_t continuity = CONTINUITY;

/*
 *     5bit      3bit 
 * |threshold|continuity|
 * 
 * threshold: 0 ~ 31
 * continuity: 0 ~ 7
 */
void set_handler(uint8_t value) {
    threshold = value / 8;
    continuity = value % 8;
}

void inv_handler(void) {
    PROTOCOL_Send_uint16_t(sum);
    sum = 0;
}
    
void loop_func(void) {
    __delay_ms(INTERVAL);
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
            
            if (v >= threshold) count++;
            else count = 0;
            
            if (count >= continuity) {
                count = 0;
                on = true;
            }
            
            break;

        case true:
            
            if (v < threshold) count++;
            else count = 0;
            
            if (count >= continuity) {
                count = 0;
                on = false;
            LATCbits.LATC7 = 0;
            __delay_ms(3);
            LATCbits.LATC7 = 1;
            sum++;
            }
            
            break;
    }
    //printf("out:%d, v:%d, on:%d, count:%d\n", out, v, on, count);
}

void main(void)
{    
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    ADC_Initialize();

    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, set_handler, inv_handler, 1);
    PROTOCOL_Set_Func(loop_func);
    
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
