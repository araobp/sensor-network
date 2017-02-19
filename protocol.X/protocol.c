#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

// function pointers
void (*PROTOCOL_Start_Handler)(void);
void (*PROTOCOL_Stop_Handler)(void);
void (*PROTOCOL_Set_Handler)(uint8_t value);

uint8_t c;
uint8_t cnt = 0;
uint8_t buf[16];
uint8_t interval;
uint8_t setting;
const char *device_id_;

// initialization
void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_handler, void *set_handler) {
    device_id_ = device_id;
    PROTOCOL_Start_Handler = start_handler;
    PROTOCOL_Stop_Handler = stop_handler;
    PROTOCOL_Set_Handler = set_handler;
}

// read data from USART
void PROTOCOL_Read() {
    do {
        c = EUSART_Read();
        buf[cnt++] = c;
        if (c == '\n') {
            buf[cnt] = '\0';
            cnt = 0;

            if (!strncmp(WHO, buf, 3)) {  // who are you?
                printf("%s\n", device_id_);
            } else if (!strncmp(INT, buf, 3)) {  // set interval
                interval = atoi(&buf[4]);
                DATAEE_WriteByte(0, interval);
            } else if (!strncmp(SAV, buf ,3)) {  // save the current setting
                DATAEE_WriteByte(0, setting);
            } else if (!strncmp(STA, buf, 3)) {  // start measurement
                // kick
            } else if (!strncmp(STP, buf, 3)) {  // stop measurement
                // kick
            } else {  // set value
                setting = atoi(buf);
                // kick
            }
        }
    } while (EUSART_DataReady);
}
