#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol_i2c.h"

typedef enum {
    TLV_SET,
    TYPE_SENT,
    LENGTH_SENT,
    COMPLETE,
    ILLEGAL
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

uint8_t slave_address;

uint8_t *data;
        
// initialization
void PROTOCOL_I2C_Initialize(uint8_t device_id) {
    slave_address = device_id;
    SSP1ADD = (device_id << 1);
}

uint8_t PROTOCOL_I2C_Who(void) {
    return slave_address;
}

void PROTOCOL_I2C_Sav(void) {
    
}

void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer) {
    readbuf.type = type;
    readbuf.length = length;
    readbuf.pbuffer = pbuffer;
    readbuf.status = TLV_SET;
    readbuf.buf_cnt = 0;
}

bool PROTOCOL_I2C_TLV_Status(void) {
    if (readbuf.status == TLV_SET) {
        return true;
    } else return false;
}

uint8_t* PROTOCOL_I2C_Sen(void) {
    uint8_t *pdata;
    switch(readbuf.status) {
        case TLV_SET:
            pdata = &readbuf.type;
            readbuf.status = TYPE_SENT;
            break;
        case TYPE_SENT:
            pdata = &readbuf.length;
            readbuf.status = LENGTH_SENT;
            break;
        case LENGTH_SENT:
            if (readbuf.buf_cnt < readbuf.length) {
                pdata = &readbuf.pbuffer[readbuf.buf_cnt++];
            }
            if (readbuf.buf_cnt == readbuf.length) readbuf.status = COMPLETE;
            break;
        default:
            readbuf.status = ILLEGAL;       
            pdata = NULL;
            break;
    }
    return pdata;
}
