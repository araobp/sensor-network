#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "protocol.h"

#define _XTAL_FREQ 1000000
#define DEVICE_ID "TOWER-PRO-SG90"

void tmr0_handler() {
     LATCbits.LATC3 ^= 1;
}

void set_handler(uint8_t angle) {
    uint16_t dutyValue = (uint16_t)(155.0 - angle * 120.0 / 180.0);
    EPWM1_LoadDutyValue(dutyValue);
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    EPWM1_Initialize();
    
    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);
    
    PROTOCOL_Initialize(DEVICE_ID, 0, 0, set_handler);
    PROTOCOL_Loop();
}
