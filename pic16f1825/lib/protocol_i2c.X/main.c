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
    uint8_t pbuf[3] = {1, 2, 3};
    while (1) {
        PROTOCOL_I2C_Set_TLV(0x11, 3, &pbuf[0]);
        __delay_ms(1000);
    }
}
