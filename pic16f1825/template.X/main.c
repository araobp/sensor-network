#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "protocol_i2c.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "TEMPLATE_DEVICE"

bool running = true;
uint8_t do_func = 0;
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;

void start_handler(void) {
    running = true;
    printf("Start Handler called\n");
}

void stop_handler(void) {
    running = false;
    printf("Stop Handler called\n");
}

void set_handler(uint8_t value) {
    period_10 = value/10;
    printf("Set Handler called\n");
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
        printf("Hello World!\n");
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

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_I2C_Initialize(TEMPLATE_I2C);
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Loop();
}