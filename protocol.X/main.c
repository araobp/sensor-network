#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>

#define DEVICE_ID "test0"

void main(void)
{
    SYSTEM_Initialize();

    uint8_t c;
    uint8_t cnt = 0;
    uint8_t buf[16];
    uint8_t interval;
    uint8_t setting;

    while (1)
    {
        do {
            c = EUSART_Read();
            buf[cnt++] = c;
            if (c == '\n') {
                buf[cnt] = '\0';
                cnt = 0;
                
                if (!strncmp(WHO, buf, 3)) {
                    printf("%s\n", DEVICE_ID);
                } else if (!strncmp(INT, buf, 3)) {
                    interval = atoi(&buf[4]);
                    DATAEE_WriteByte(0, interval);
                } else if (!strncmp(SAV, buf ,3)) {
                    DATAEE_WriteByte(0, setting);
                } else if (!strncmp(STA, buf, 3)) {
                    // kick
                } else if (!strncmp(STP, buf, 3)) {
                    // kick
                } else {
                    setting = atoi(buf);
                    // kick
                }
            }
        } while (EUSART_DataReady);
    }
}
