#include "mcc_generated_files/mcc.h"
#include "protocol_i2c.h"
#include <stdlib.h>

#define __XTAL_FREQ 500000
#define DEVICE_ID 0x01

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
  
    PROTOCOL_I2C_Initialize(DEVICE_ID);
    while (1) {
        
    }
}
