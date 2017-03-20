#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "TOWER-PRO-SG90"

uint8_t running = 1;

// Duty value at 50Hz
//  20: 3% (0.5msec)
//  80: 12% (2.4msec))
//  Range by actual measurement: 16(0 degrees) - 78(180 degrees)
#define DUTY_0 16.0
#define DUTY_180 78.0
void set_handler(uint8_t angle) {
    if (running) {
        uint16_t value = (uint16_t)(DUTY_0 + (DUTY_180 - DUTY_0) * angle / 180.0);
        EPWM1_LoadDutyValue(value);
    }
}

void start_handler(void) {
    running = 1;
}

void stop_handler(void) {
    running = 0;
}

void tmr0_handler(void) {
    if (running) {
        LATCbits.LATC3 ^= 1;
    }
}       
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    EPWM1_Initialize();
    
    EUSART_Initialize();
    
    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_Loop();
}
