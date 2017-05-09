#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 500000
#define DEVICE_ID "A1324LUA-T"

#define THRESHOLD 8
#define MIDDLE 511

bool running = true;

uint16_t sum = 0;
static uint16_t cnt = 0;
bool on = false;

uint8_t tmr_cnt = 0;
uint8_t period = 0;
uint16_t sec_cnt = 0;

void start_handler(void) {
    LATCbits.LATC7 = 1;
    running = true;
}

void stop_handler(void) {
    running = false;
    LATCbits.LATC7 = 1;
}

void set_handler(uint8_t value) {
    period = value;
    sum = 0;
    cnt = 0;
    tmr_cnt = 0;
    sec_cnt = 0;
}

//void tmr0_handler(void) {
void loop_func(void) {

    if (LATCbits.LATC7 == 0) LATCbits.LATC7 = 1;

    if (running) {
        // Out
        ADC_SelectChannel(channel_AN8);
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
                LATCbits.LATC7 = 0;
                __delay_ms(5);
                if (period == 0) {
                    //printf("1\n");
                    PROTOCOL_Send_uint16_t(1);
                } else {
                    ++sum;
                }
            }
        }
        if (TMR0_HasOverflowOccured()) {
            ++tmr_cnt;
            TMR0IF = 0;
        }
        if (tmr_cnt >= 2) {  // 500msec * 2 = 1sec
            tmr_cnt = 0;
            if (period > 0 && ++sec_cnt >= period) {
                //printf("%d\n", sum);
                PROTOCOL_Send_uint16_t(sum);
                sum = 0;
                sec_cnt = 0;
            }
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
    
    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Set_Func(loop_func);

    EUSART_Initialize();
    TMR0_Initialize();
    // TMR0_SetInterruptHandler(tmr0_handler);
    
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    I2C1_Initialize();

    PROTOCOL_Loop();
}