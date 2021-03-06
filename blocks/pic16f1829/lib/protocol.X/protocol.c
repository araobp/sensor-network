#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#define I2C_SLAVE_MASK 0x7F

#define parse(cmd_name, buf) !strncmp((cmd_name), (buf), 3)

uint8_t slave_address;
        
// function pointers (event handlers)
void (*PROTOCOL_Start_Handler)(void) = NULL;
void (*PROTOCOL_Stop_Handler)(void) = NULL;
void (*PROTOCOL_Set_Handler)(uint8_t value) = NULL;
void (*PROTOCOL_Loop_Func)(void) = NULL;
void (*PROTOCOL_Extension_Handler)(char*) = NULL;
void (*PROTOCOL_Inv_Handler)(void) = NULL;
void (*PROTOCOL_Tick_Handler)(void) = NULL;

uint8_t cnt = 0;
uint8_t buf[BUF_SIZE];
uint8_t value;
const char *device_id_;
bool locked = false;
bool running = false;
bool invoked = false;
uint8_t sec_cnt = 0;
uint8_t tmr_cnt = 0;
uint8_t tmr_scaler = 1;
bool tmr_overflow = false;
uint8_t type_no_data = TYPE_NO_DATA;

// I2C backplane
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

bool backplane_slave_enabled = false;
uint8_t sendbuf[16];
uint8_t i, j;
int16_t float100;        
uint8_t *data;

// initialization
void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_handler, void *set_handler, void *inv_handler, uint8_t scaler) {
    device_id_ = device_id;
    PROTOCOL_Start_Handler = start_handler;
    PROTOCOL_Stop_Handler = stop_handler;
    PROTOCOL_Set_Handler = set_handler;
    
    if (PROTOCOL_Stop_Handler) PROTOCOL_Stop_Handler();
    value = DATAEE_ReadByte(DEVICE_SETTING_ADDRESS);  // read value from EEPROM
    if (PROTOCOL_Set_Handler) PROTOCOL_Set_Handler(value);
    slave_address = DATAEE_ReadByte(DEVICE_ID_I2C_ADDRESS);  // read slave_address from EEPROM

    // I2C backplane initalization
    readbuf.status = COMPLETE;
    PROTOCOL_Inv_Handler = inv_handler;
    tmr_scaler = scaler;
}

void PROTOCOL_Set_Func(void *loop_func) {
    PROTOCOL_Loop_Func = loop_func;
}

void PROTOCOL_Set_Extension_Handler(void *extension_handler) {
    PROTOCOL_Extension_Handler = extension_handler;
}

void PROTOCOL_Set_Tick_Handler(void *tick_handler) {
    PROTOCOL_Tick_Handler = tick_handler;
}

void PROTOCOL_Write_Device_Address(uint8_t device_id_i2c) {
    DATAEE_WriteByte(DEVICE_ID_I2C_ADDRESS, device_id_i2c);
    slave_address = device_id_i2c;
}

uint8_t PROTOCOL_Read_Device_Address() {
    return slave_address;
}

void PROTOCOL_STA(void) {
    if (PROTOCOL_Start_Handler) PROTOCOL_Start_Handler();
    running = true;
}

void PROTOCOL_STP(void) {
    if (PROTOCOL_Stop_Handler) PROTOCOL_Stop_Handler();
    running = false;
}

void PROTOCOL_INV(void) {
    invoked = true;
}

void PROTOCOL_SAV() {
    DATAEE_WriteByte(DEVICE_SETTING_ADDRESS, value);
}

void PROTOCOL_SET(uint8_t value_) {
    value = value_;
    if (PROTOCOL_Set_Handler) PROTOCOL_Set_Handler(value);
}

void PROTOCOL_EXT(char *char_buf) {
    if (PROTOCOL_Extension_Handler) PROTOCOL_Extension_Handler(char_buf);
}

void PROTOCOL_Set_Lock(bool lock) {
    locked = lock;
}

bool PROTOCOL_Read_Lock(void) {
    return locked;
}

/*
 * Loop for device invocation and USART Rx reader
 */
void PROTOCOL_Loop() {
    uint8_t device_address;
    uint8_t c;
    while (1) {
        tmr_overflow = TMR0_HasOverflowOccured();
        if (tmr_overflow) {
            TMR0IF = 0;
            if (PROTOCOL_Tick_Handler) PROTOCOL_Tick_Handler();
            if (PROTOCOL_Inv_Handler && (++tmr_cnt >= value)) {
                tmr_cnt = 0;
                if (++sec_cnt >= tmr_scaler) {
                    if (running) invoked = true;
                    sec_cnt = 0;
                }
            }
        }
        if (invoked && PROTOCOL_Inv_Handler) {
            PROTOCOL_Inv_Handler();
            invoked = false;
        }
        if (PROTOCOL_Loop_Func) PROTOCOL_Loop_Func();
        if (EUSART_DataReady) {
            c = EUSART_Read();
            buf[cnt++] = c;
            if (c == '\n') {
                buf[--cnt] = '\0';
                cnt = 0;
                printf("#%s\n", buf);
                if (BACKPLANE_SLAVE_ADDRESS != BACKPLANE_MASTER_I2C) {  // Handled by backplane master?
                    PROTOCOL_EXT(buf);                    
                } else if (parse(WHO, buf)) {  // who are you?
                    printf("$:WHO:%s\n", device_id_);
                } else if (parse(SAV, buf)) {  // save the current setting
                    PROTOCOL_SAV();
                } else if (parse(STA, buf)) {  // start measurement
                    PROTOCOL_STA();
                } else if (parse(STP, buf)) {  // stop measurement
                    PROTOCOL_STP();
                    printf("*:STP:%s\n", ACK);
                } else if (parse(SET, buf)) {  // set value
                    PROTOCOL_SET(atoi(&buf[4]));
                } else if (parse(GET, buf)) {  // get value
                    printf("$:GET:%d\n", value);
                } else if (parse(WDA, buf)) {
                    device_address = atoi(&buf[4]);
                    PROTOCOL_Write_Device_Address(device_address);
                } else if (parse(RDA, buf)) {
                    device_address = PROTOCOL_Read_Device_Address();
                    printf("$:RDA:%d\n", device_address);
                } else {
                    PROTOCOL_EXT(buf);
                }
            }
        }
    }
}

/************************************************************/
/* Backplane I2C slave                                      */
/************************************************************/

uint16_t concat(uint8_t msb, uint8_t lsb) {
    return (uint16_t)msb * 256 + (uint16_t)lsb;    
}

void PROTOCOL_Backplane_Slave_Enabled(void) {
    backplane_slave_enabled = true;
}

uint8_t PROTOCOL_I2C_WHO(void) {
    return slave_address;
}

uint8_t PROTOCOL_I2C_GET(void) {
    return value;
}

void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer) {
    READBUF_STATUS status = readbuf.status;
    if (status == COMPLETE) {
        readbuf.type = type;
        readbuf.length = length;
        readbuf.pbuffer = pbuffer;
        readbuf.buf_cnt = 0;
        readbuf.status = TLV_SET;
    }
}

void PROTOCOL_Print_TLV(uint8_t dev_addr, uint8_t type, uint8_t length, uint8_t *pbuffer) {
    int16_t v;
    if (dev_addr != 0) printf("%%%d:", dev_addr);
    switch(type) {
        case TYPE_UINT8_T:
            length--;
            printf("UINT8_T:");
            for (i=0; i<length; i++) printf("%u,", pbuffer[i]);
            printf("%u\n", pbuffer[i]);
            break;
        case TYPE_INT8_T:
            length--;
            printf("INT8_T:");
            for (i=0; i<length; i++) printf("%d,", (int8_t)pbuffer[i]);
            printf("%d\n", (int8_t)pbuffer[i]);            
            break;
        case TYPE_UINT16_T:
            printf("UINT16_T:");
            length = length - 2;
            for(i=0; i<length; i=i+2) printf("%u,", concat(pbuffer[i], pbuffer[i+1]));
            printf("%u\n", concat(pbuffer[i], pbuffer[i+1]));
            break;
        case TYPE_INT16_T:
            printf("INT16_T:");
            length = length - 2;
            for(i=0; i<length; i=i+2) printf("%d,", (int16_t)(concat(pbuffer[i], pbuffer[i+1])));
            printf("%d\n", (int16_t)(concat(pbuffer[i], pbuffer[i+1])));                        
            break;
        case TYPE_FLOAT:
            printf("FLOAT:");
            length = length - 2;
            for (i=0; i<length; i=i+2) {
                v = (int16_t)(concat(pbuffer[i], pbuffer[i+1]));
                if (v <= -100 || v >= 0) {
                    printf("%d.%02d,", v/100, abs(v%100));
                } else {
                    printf("-%d.%02d,", v/100, abs(v%100));                    
                }
            }
            v = (int16_t)(concat(pbuffer[i], pbuffer[i+1]));
            if (v <= -100 || v >= 0) {
                printf("%d.%02d\n", v/100, abs(v%100));
            } else {
                printf("-%d.%02d\n", v/100, abs(v%100));                
            }
            break;
        case TYPE_NO_DATA:
            printf("NO_DATA\n");
            break;
        default:
            printf("ILLEGAL_TYPE\n");
            break;
    }
}

bool PROTOCOL_I2C_TLV_Status(void) {
    if (readbuf.status == TLV_SET) {
        return true;
    } else return false;
}

void PROTOCOL_RST(void) {
    if (readbuf.status != TLV_SET) {
        readbuf.buf_cnt = 0;
        readbuf.status = COMPLETE;
    }
}

void PROTOCOL_I2C_Send_uint8_t(uint8_t length, uint8_t *pbuffer) {
    if (backplane_slave_enabled) {
        PROTOCOL_I2C_Set_TLV(TYPE_UINT8_T, length, &pbuffer[0]);
    } else {
        length--;
        for(i=0; i<length; i++) printf("%d,", pbuffer[i]);
        printf("%d\n", pbuffer[i]);
    }
}

void PROTOCOL_I2C_Send_int8_t(uint8_t length, int8_t *pbuffer) {
    if (backplane_slave_enabled) {
        for (i=0; i<length; i++) {
            sendbuf[i] = (uint8_t)pbuffer[i];
        }
        PROTOCOL_I2C_Set_TLV(TYPE_INT8_T, length, &sendbuf[0]);
    } else {
        length--;
        for (i=0; i<length; i++) printf("%d,", (int8_t)pbuffer[i]);
        printf("%d\n", (int8_t)pbuffer[i]);
    }
}

void PROTOCOL_I2C_Send_uint16_t(uint8_t length, uint16_t *pbuffer) {
    if (backplane_slave_enabled) {
        j = 0;
        for(i=0; i<length; i++) {
            sendbuf[j++] = (uint8_t)((pbuffer[i] >> 8) & 0x00ff);
            sendbuf[j++] = (uint8_t)(pbuffer[i] & 0x00ff);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_UINT16_T, length*2, &sendbuf[0]);
    } else {
        length--;
        for(i=0; i<length; i++) printf("%u,", pbuffer[i]);
        printf("%u\n", pbuffer[i]);
    }
}

void PROTOCOL_I2C_Send_int16_t(uint8_t length, int16_t *pbuffer) {
    if (backplane_slave_enabled) {
        j = 0;
        for (i=0; i<length; i++) {
            sendbuf[j++] = (uint8_t)((pbuffer[i] >> 8) & 0x00ff);
            sendbuf[j++] = (uint8_t)(pbuffer[i] & 0x00ff);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_INT16_T, length*2, &sendbuf[0]);
    } else {
        length--;
        for(i=0; i<length; i++) printf("%d,", pbuffer[i]);
        printf("%d\n", pbuffer[i]);                        
    }
}

void PROTOCOL_I2C_Send_float(uint8_t length, float *pbuffer) {
    int16_t v;
    if (backplane_slave_enabled) {
        j = 0;
        for (i=0; i<length; i++) {
            float100 = (int16_t)(pbuffer[i] * 100);
            sendbuf[j++] = (uint8_t)((float100 >> 8) & 0x00ff);
            sendbuf[j++] = (uint8_t)(float100 & 0x00ff);
        }
        PROTOCOL_I2C_Set_TLV(TYPE_FLOAT, length*2, &sendbuf[0]);
    } else {
        length--;
        for (i=0; i<length; i++) {
            v = (int16_t)(pbuffer[i] * 100);
            if (v <= -100 || v >= 0) {
                printf("%d.%02d,", v/100, abs(v%100));
            } else {
                printf("-%d.%02d,", v/100, abs(v%100));                
            }
        }
        v = (int16_t)(pbuffer[i] * 100);
        if (v <= -100 || v >= 0) {
            printf("%d.%02d\n", v/100, abs(v%100));
        } else {
            printf("-%d.%02d\n", v/100, abs(v%100));            
        }
    }
}

void PROTOCOL_Send_uint8_t(uint8_t value) {
    static uint8_t pbuf[1];
    pbuf[0] = value;
    PROTOCOL_I2C_Send_uint8_t(1, pbuf);
}

void PROTOCOL_Send_int8_t(int8_t value) {
    static int8_t pbuf[1];
    pbuf[0] = value;
    PROTOCOL_I2C_Send_int8_t(1, pbuf);
}

void PROTOCOL_Send_uint16_t(uint16_t value) {
    static uint16_t pbuf[1];
    pbuf[0] = value;
    PROTOCOL_I2C_Send_uint16_t(1, pbuf);
}

void PROTOCOL_Send_int16_t(int16_t value) {
    static int16_t pbuf[1];
    pbuf[0] = value;
    PROTOCOL_I2C_Send_int16_t(1, pbuf);
}

void PROTOCOL_Send_float(float value) {
    static float pbuf[1];
    pbuf[0] = value;
    PROTOCOL_I2C_Send_float(1, pbuf);
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
                readbuf.buf_cnt = 0;
                readbuf.status = COMPLETE;
            }
            break;
        case COMPLETE:
            pdata = &type_no_data;
            break;
    }
    return pdata;
}
