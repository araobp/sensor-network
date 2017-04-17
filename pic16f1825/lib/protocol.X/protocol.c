#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

// function pointers (event handlers)
void (*PROTOCOL_Start_Handler)(void) = NULL;
void (*PROTOCOL_Stop_Handler)(void) = NULL;
void (*PROTOCOL_Set_Handler)(uint8_t value) = NULL;
void (*PROTOCOL_Loop_Func)(void) = NULL;
void (*PROTOCOL_Extension_Handler)(uint8_t *buf) = NULL;

uint8_t c;
uint8_t cnt = 0;
uint8_t buf[48];
uint8_t value;
const char *device_id_;

// initialization
void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_handler, void *set_handler) {
    device_id_ = device_id;
    PROTOCOL_Start_Handler = start_handler;
    PROTOCOL_Stop_Handler = stop_handler;
    PROTOCOL_Set_Handler = set_handler;
    
    if (PROTOCOL_Stop_Handler) PROTOCOL_Stop_Handler();
    value = DATAEE_ReadByte(DEVICE_SETTING_ADDRESS);  // read value from EEPROM
    if (PROTOCOL_Set_Handler) PROTOCOL_Set_Handler(value);
    if (PROTOCOL_Start_Handler) PROTOCOL_Start_Handler();
}

void PROTOCOL_Set_Func(void *loop_func) {
    PROTOCOL_Loop_Func = loop_func;
}

void PROTOCOL_Set_Extension_Handler(void *extension_handler) {
    PROTOCOL_Extension_Handler = extension_handler;
}

void PROTOCOL_Write_Device_Address(uint8_t device_id_i2c) {
    DATAEE_WriteByte(DEVICE_ID_I2C_ADDRESS, device_id_i2c);
}

uint8_t PROTOCOL_Read_Device_Address() {
    return DATAEE_ReadByte(DEVICE_ID_I2C_ADDRESS);
}

void PROTOCOL_STA(void) {
    PROTOCOL_Start_Handler();
}

void PROTOCOL_STP(void) {
    PROTOCOL_Stop_Handler();
}

void PROTOCOL_SAV() {
    DATAEE_WriteByte(DEVICE_SETTING_ADDRESS, value);
}

void PROTOCOL_SET(uint8_t value_) {
    value = value_;
    PROTOCOL_Set_Handler(value);
}

/*
 * USART Rx reader
 */
void PROTOCOL_Loop() {
    uint8_t device_address;
    while (1) {
        if (PROTOCOL_Loop_Func) PROTOCOL_Loop_Func();
        if (EUSART_DataReady) {
            c = EUSART_Read();
            buf[cnt++] = c;
            if (c == '\n') {
                buf[--cnt] = '\0';
                cnt = 0;

                if (CLI_SLAVE_ADDRESS) {  // Handled by backplane master?
                    PROTOCOL_Extension_Handler(buf);                    
                } else if (!strncmp(WHO, buf, 3)) {  // who are you?
                    printf("%s\n", device_id_);
                } else if (!strncmp(SAV, buf ,3)) {  // save the current setting
                    PROTOCOL_SAV();
                } else if (!strncmp(STA, buf, 3)) {  // start measurement
                    PROTOCOL_STA();
                } else if (!strncmp(STP, buf, 3)) {  // stop measurement
                    PROTOCOL_STP();
                    printf("%s\n", ACK);
                } else if (!strncmp(SET, buf, 3)) {  // set value
                    PROTOCOL_SET(atoi(&buf[4]));
                } else if (!strncmp(GET, buf, 3)) {  // get value
                    printf("VAL:%d\n", value);
                } else if (!strncmp(WDA, buf, 3)) {
                    device_address = atoi(&buf[4]);
                    PROTOCOL_Write_Device_Address(device_address);
                } else if (!strncmp(RDA, buf, 3)) {
                    device_address = PROTOCOL_Read_Device_Address();
                    printf("I2C:%d\n", device_address);
                }
                else {
                    PROTOCOL_Extension_Handler(buf);
                }
            }
        };
    }
}

/************************************************************/
/* Backplane I2C slave                                      */
/************************************************************/

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

bool backplane_slave_enabled = false;
uint8_t slave_address;
uint8_t sendbuf[16];
uint8_t i, j;
int16_t float100;
        
uint8_t *data;
        
// initialization
void PROTOCOL_I2C_Initialize(uint8_t device_id) {
    backplane_slave_enabled = true;
    slave_address = device_id;
    SSP1ADD = (device_id << 1);
}

uint8_t PROTOCOL_I2C_WHO(void) {
    return slave_address;
}

uint8_t PROTOCOL_I2C_GET(void) {
    return value;
}

void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer) {
    if (readbuf.status == TLV_SET || readbuf.status == COMPLETE || readbuf.status == ILLEGAL) {
        readbuf.type = type;
        readbuf.length = length;
        readbuf.pbuffer = pbuffer;
        readbuf.status = TLV_SET;
        readbuf.buf_cnt = 0;
    }
}

bool PROTOCOL_I2C_TLV_Status(void) {
    if (readbuf.status == TLV_SET) {
        return true;
    } else return false;
}

void PROTOCOL_I2C_Send_uint8_t(uint8_t length, uint8_t *pbuffer) {
    if (backplane_slave_enabled) {
        PROTOCOL_I2C_Set_TLV(TYPE_UINT8_T, length, &pbuffer[0]);
    }
}

void PROTOCOL_I2C_Send_int8_t(uint8_t length, int8_t *pbuffer) {
    if (backplane_slave_enabled) {
        for (i=0; i<length; i++) {
            sendbuf[i] = (uint8_t)pbuffer[i];
        }
        PROTOCOL_I2C_Set_TLV(TYPE_INT8_T, length, &sendbuf[0]);
    }
}

void PROTOCOL_I2C_Send_uint16_t(uint8_t length, uint16_t *pbuffer) {
    if (backplane_slave_enabled) {
        j = 0;
        for (i=0; i<length; i++) {
            sendbuf[j] = (uint8_t)(pbuffer[i] & 0x00ff);
            sendbuf[++j] = (uint8_t)(pbuffer[i] >> 8);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_UINT16_T, length, &sendbuf[0]);
    }
}

void PROTOCOL_I2C_Send_int16_t(uint8_t length, int16_t *pbuffer) {
    if (backplane_slave_enabled) {
        j = 0;
        for (i=0; i<length; i++) {
            sendbuf[j] = (uint8_t)(pbuffer[i] & 0x00ff);
            sendbuf[++j] = (uint8_t)(pbuffer[i] >> 8 & 0x00ff);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_INT16_T, length, &sendbuf[0]);
    }
}

void PROTOCOL_I2C_Send_float(uint8_t length, float *pbuffer) {
    if (backplane_slave_enabled) {
        j = 0;
        for (i=0; i<length; i++) {
            float100 = (int16_t)(pbuffer[i] * 100);
            sendbuf[j] = (uint8_t)(float100 & 0x00ff);
            sendbuf[++j] = (uint8_t)(float100 >> 8 & 0x00ff);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_FLOAT, length, &sendbuf[0]);
    }   
}

uint8_t* PROTOCOL_I2C_SEN(void) {
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
            if (readbuf.buf_cnt == readbuf.length) {
                readbuf.status = COMPLETE;
                readbuf.buf_cnt = 0;
            }
            break;
        default:
            readbuf.status = ILLEGAL;       
            pdata = NULL;
            break;
    }
    return pdata;
}
