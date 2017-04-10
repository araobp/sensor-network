#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol_i2c.h"

typedef enum {
    TLV_SET,
    TYPE_SENT,
    LENGTH_SENT,
    COMPLETE
} READBUF_STATUS;

typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t *pbuffer;
    READBUF_STATUS status;
    uint8_t buf_cnt;
} READBUF;

READBUF readbuf;
READBUF_STATUS readbuf_status;

uint8_t device_id_;

uint8_t *data;
        
// initialization
void PROTOCOL_I2C_Initialize(uint8_t device_id) {
    device_id_ = device_id;
}

uint8_t PROTOCOL_I2C_Who(void) {
    return device_id_;
}

void PROTOCOL_I2C_Sav(void) {
    
}

void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer) {
    readbuf.type = type;
    readbuf.length = length;
    readbuf.pbuffer = pbuffer;
    readbuf.status = TLV_SET;
    readbuf.buf_cnt = length;
}

uint8_t* PROTOCOL_I2C_Sen() {
    uint8_t *pdata;
    switch(readbuf.status) {
        case TLV_SET:
            pdata = &readbuf.type;
            break;
        case TYPE_SENT:
            pdata = &readbuf.length;
            break;
        case LENGTH_SENT:
            if (readbuf.buf_cnt > 0) {
                pdata = &readbuf.pbuffer[--readbuf.buf_cnt];
            } else {
                data = NULL;
            }
    }
    return data;
}
