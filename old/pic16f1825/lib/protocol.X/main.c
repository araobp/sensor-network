#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>

#define __XTAL_FREQ 500000
#define DEVICE_ID "sensor1"

uint8_t value;
uint8_t period_10;
uint8_t timer_cnt = 0;
uint8_t running = 0;

void start_handler(void) {
    printf("start handler called\n");
    running = 1;
}

void stop_handler(void) {
    printf("stop handler called\n");
    running = 0;
}

void set_handler(uint8_t value) {
    printf("set handler called, value = %d\n", value);
    period_10 = value/10;
}

// called at every 99.84msec
void tmr0_handler(void) {
    if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) {
        // blink LED
           LATCbits.LATC3 ^= 1;
        }
    }
    CLRWDT();
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
  
    value = DATAEE_ReadByte(0);
    if (value < 10) value = 10;
    period_10 = value/10;
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Loop();
}
