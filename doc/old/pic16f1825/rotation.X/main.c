#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "A1324LUA-T"

#define THRESHOLD 8
#define MIDDLE 511

uint8_t running = 1;

uint16_t sum = 0;
static uint16_t cnt = 0;
bool on = false;

uint8_t tmr_cnt = 0;
uint8_t period = 0;
uint16_t sec_cnt = 0;

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

void set_handler(uint8_t value) {
    period = value;
    sum = 0;
    cnt = 0;
    tmr_cnt = 0;
    sec_cnt = 0;
}

void tmr0_handler(void) {  // 5msec timer
    if (LATCbits.LATC3 == 1) LATCbits.LATC3 = 0;
        
    if (running) {
        // Out
        ADC_SelectChannel(channel_AN6);
        ADC_StartConversion();
        while(!ADC_IsConversionDone());
        adc_result_t out = ADC_GetConversionResult();

        uint16_t v = abs(out - MIDDLE);
        if (on == false && v > THRESHOLD) {
            cnt++;
            if (cnt > 3) {
                cnt = 0;
                on = true;
            }
        } else if (on == true && v < THRESHOLD) {
            cnt++;
            if (cnt > 3) {
                cnt = 0;
                on = false;
                LATCbits.LATC3 = 1;
                if (period == 0) {
                    printf("1\n");
                } else {
                    ++sum;
                }
            }
        }

        if (++tmr_cnt >= 200) {  // 5msec * 200 = 1sec
            tmr_cnt = 0;
            if (period > 0 && ++sec_cnt >= period) {
                printf("%d\n", sum);
                sum = 0;
                sec_cnt = 0;
            }
        }
    }
}

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
    PROTOCOL_Loop();
}
