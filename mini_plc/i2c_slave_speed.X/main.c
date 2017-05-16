#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"
#include <stdlib.h>

#define _XTAL_FREQ 500000

#define DEVICE_ID "A1324LUA_T"

#define THRESHOLD 8
#define MIDDLE 511

bool on = false;
uint16_t sum = 0;
uint16_t count = 0;

void inv_handler(void) {
    PROTOCOL_Send_uint16_t(sum);
    sum = 0;
}
    
void loop_func(void) {
    __delay_ms(10);
    ADC_SelectChannel(channel_AN8);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());
    adc_result_t out = ADC_GetConversionResult();

    uint16_t v = abs(out - MIDDLE);
    if (on == false && v > THRESHOLD) {
        count++;
        if (count > 3) {
            count = 0;
            on = true;
        }
    } else if (on == true && v < THRESHOLD) {
        count++;
        if (count > 3) {
            count = 0;
            on = false;
            LATCbits.LATC7 = 0;
            __delay_ms(3);
            LATCbits.LATC7 = 1;
            ++sum;
        }
    }
}

void main(void)
{    
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    ADC_Initialize();

    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, inv_handler, 20);
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
