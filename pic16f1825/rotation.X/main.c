#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "A1324LUA-T"

#define THRESHOLD 8
#define MIDDLE 511

uint8_t running = 1;

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

static uint16_t cnt = 0;
uint16_t sum_out = 0;
bool on = false;
uint8_t led_cnt = 0;

void tmr0_handler(void) {
    if (LATCbits.LATC3 == 1 && ++led_cnt >= 10) {
        LATCbits.LATC3 = 0;
    }

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
                printf("1\n");
            }
        }
    //printf("%d %d %d\n", v, cnt, on);
    }
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    ADC_Initialize();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    EUSART_Initialize();
    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, 0);
    PROTOCOL_Loop();
}
