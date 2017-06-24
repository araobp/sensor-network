#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>

#define __XTAL_FREQ 500000
#define SLAVE_ADDRESS 0x12
#define DEVICE_ID "TEST_SLAVE"

uint8_t running = true;

void start_handler(void) {
    running = true;
    printf("Start handler called\n");
}

void stop_handler(void) {
    running = false;
    printf("Stop handler called\n");
}

void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
  
    PROTOCOL_I2C_Initialize(SLAVE_ADDRESS);
    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, NULL);
    uint8_t pbuf[3] = {1, 2, 3};
    while (1) {
        if (running) {
            PROTOCOL_I2C_Set_TLV(0x11, 3, &pbuf[0]);
            __delay_ms(1000);
        }
    }
}
