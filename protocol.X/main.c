#include "mcc_generated_files/mcc.h"
#include <stdlib.h>

void main(void)
{
    SYSTEM_Initialize();

    uint8_t c;
    uint8_t cnt;
    uint8_t buf[4];
    uint8_t byte_value;

    while (1)
    {
        do {
            c = EUSART_Read();
            // printf("%c\n", c);
            if (c == '\n') {
                
                buf[cnt] = '\0';
                cnt = 0;
                byte_value = atoi(buf);
                DATAEE_WriteByte(0, byte_value);
                // printf("New Period: %s\n", buf);
            } else if (cnt < 3) {
                buf[cnt++] = c;
            } else {
                cnt = 0;
                break;
            }
        } while (EUSART_DataReady);
    }
}
