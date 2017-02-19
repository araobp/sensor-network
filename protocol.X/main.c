#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>

#define DEVICE_ID "sensor1"

const char *device_id = DEVICE_ID;

void start_handler(void) {
    printf("start handler called\n");
}

void stop_handler(void) {
    printf("stop handler called\n");
}

void set_handler(uint8_t value) {
    printf("set handler called, value = %d\n", value);
}

void main(void)
{
    SYSTEM_Initialize();
    
    PROTOCOL_Initialize(device_id, start_handler, stop_handler, set_handler);
    
    while (1)
    {
        PROTOCOL_Read();
    }
}

