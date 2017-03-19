#include <pic16f1825.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "TOWER-PRO-SG90"

// Duty value at 50Hz
//  20: 3% (0.5msec)
//  80: 12% (2.4msec))
//  Range by actual measurement: 16(0 degrees) - 78(180 degrees)
#define DUTY_0 16.0
#define DUTY_180 78.0
void set_handler(uint8_t angle) {
    uint16_t value = DUTY_0 + (DUTY_180 - DUTY_0) * angle / 180.0;
    EPWM1_LoadDutyValue(value);
}

void tmr0_handler(void) {
    LATCbits.LATC3 ^= 1;
}       
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    EPWM1_Initialize();
    
    EUSART_Initialize();
    PROTOCOL_Initialize(DEVICE_ID, 0, 0, set_handler);
    PROTOCOL_Loop();
}
