#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#define I2C_SLAVE_ADDRESS 0x01 
#define I2C_SLAVE_MASK    0x7F

#define DEFAULT    0x00
#define SET_VALUE  0x01
#define EXT_LENGTH 0x02
#define EXT_VALUE  0x03

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

/************************************************************/
/* I2C slave (originally i2c.c)                             */
/************************************************************/

typedef enum
{
    SLAVE_NORMAL_DATA,
    SLAVE_DATA_ADDRESS,
    SLAVE_GENERAL_CALL,
} SLAVE_WRITE_DATA_TYPE;

/**
 Section: Global Variables
*/

volatile uint8_t    I2C_slaveWriteData      = 0x55;

/**
 Section: Local Functions
*/
void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state);

void I2C_Initialize(void)
{
    
    // delay 2 sec for the master to finish its start up process
    __delay_ms(2000);

    // initialize the hardware
    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP Standard Speed; UA dontupdate; CKE disabled; D_nA lastbyte_address; 
    SSP1STAT = 0x80;
    // SSPEN enabled; WCOL no_collision; CKP disabled; SSPM 7 Bit Polling; SSPOV no_overflow; 
    SSP1CON1 = 0x26;
    // ACKEN disabled; GCEN enabled; PEN disabled; ACKDT acknowledge; RSEN disabled; RCEN disabled; ACKSTAT received; SEN enabled; 
    SSP1CON2 = 0x81;
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    // SSPMSK 127; 
    SSP1MSK = (I2C_SLAVE_MASK << 1);  // adjust UI mask for R/nW bit            
    // SSPADD 8; 
    SSP1ADD = (PROTOCOL_Read_Device_Address() << 1);  // adjust UI address for R/nW bit

    // clear the slave interrupt flag
    PIR1bits.SSP1IF = 0;
    // enable the master interrupt
    PIE1bits.SSP1IE = 1;

}

void I2C_ISR ( void )
{
    uint8_t     i2c_data                = 0x55;

    // NOTE: The slave driver will always acknowledge
    //       any address match.

    PIR1bits.SSP1IF = 0;        // clear the slave interrupt flag
    i2c_data        = SSP1BUF;  // read SSPBUF to clear BF
    if(1 == SSP1STATbits.R_nW)
    {
        if((1 == SSP1STATbits.D_nA) && (1 == SSP1CON2bits.ACKSTAT))
        {
            // callback routine can perform any post-read processing
            I2C_StatusCallback(I2C_SLAVE_READ_COMPLETED);
        }
        else
        {
            // callback routine should write data into SSPBUF
            I2C_StatusCallback(I2C_SLAVE_READ_REQUEST);
        }
    }
    else if(0 == SSP1STATbits.D_nA)
    {
        // this is an I2C address

        if(0x00 == i2c_data)
        {
            // this is the General Call address
            I2C_StatusCallback(I2C_SLAVE_GENERAL_CALL_REQUEST);
        }
        else
        {
            // callback routine should prepare to receive data from the master
            I2C_StatusCallback(I2C_SLAVE_WRITE_REQUEST);
        }
    }
    else
    {
        I2C_slaveWriteData   = i2c_data;

        // callback routine should process I2C_slaveWriteData from the master
        I2C_StatusCallback(I2C_SLAVE_WRITE_COMPLETED);
    }

    SSP1CON1bits.CKP    = 1;    // release SCL

}

void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state)
{

    static uint8_t slaveWriteType   = SLAVE_NORMAL_DATA;
    static uint8_t next = DEFAULT;
    static uint8_t ext_len = 0;
    static uint8_t ext_cnt = 0;
    static char ext_buf[BUF_SIZE];
    uint8_t *pdata;
    switch (i2c_bus_state)
    {
        case I2C_SLAVE_WRITE_REQUEST:
            slaveWriteType  = SLAVE_DATA_ADDRESS;
            break;

        case I2C_SLAVE_GENERAL_CALL_REQUEST:
            // the master will be sending general call data next
            slaveWriteType  = SLAVE_GENERAL_CALL;
            break;

        case I2C_SLAVE_WRITE_COMPLETED:

            switch(slaveWriteType)
            {
                case SLAVE_DATA_ADDRESS:
                    switch(next) {
                        case SET_VALUE:
                            PROTOCOL_SET(I2C_slaveWriteData);
                            next = DEFAULT;
                            break;
                        case DEFAULT:
                            switch(I2C_slaveWriteData) {
                                case STA_I2C:
                                    PROTOCOL_STA();
                                    break;
                                case STP_I2C:
                                    PROTOCOL_STP();
                                    break;
                                case SAV_I2C:
                                    PROTOCOL_SAV();
                                    break;
                                case INV_I2C:
                                    PROTOCOL_INV();
                                    break;
                                case RST_I2C:
                                    PROTOCOL_RST();
                                    break;
                                case SET_I2C:
                                    next = SET_VALUE;
                                    break;
                                case EXT_I2C:
                                    next = EXT_LENGTH;
                                    break;
                            }
                            break;
                    }
                    break;

                case SLAVE_GENERAL_CALL:
                    if (I2C_slaveWriteData == PLG_I2C) {
                        SSP1CON2bits.GCEN = 0;  // Disable General Call reception
                        PROTOCOL_Backplane_Slave_Enabled();
                    }
                    break;

                case SLAVE_NORMAL_DATA:
                    switch(next) {
                        case EXT_LENGTH:
                            ext_len = I2C_slaveWriteData;
                            ext_cnt = 0;
                            next = EXT_VALUE;
                            break;
                        case EXT_VALUE:
                            ext_buf[ext_cnt++] = (char)I2C_slaveWriteData;
                            if (ext_cnt >= ext_len) {
                                if (!PROTOCOL_Read_Lock()) PROTOCOL_EXT(&ext_buf[0]);
                                next = DEFAULT;
                            }
                            break;
                    }
                    break;
                default:
                    break;

            }

            slaveWriteType  = SLAVE_NORMAL_DATA;
            break;

        case I2C_SLAVE_READ_REQUEST:
            switch (I2C_slaveWriteData)
            {
                case WHO_I2C:
                    SSP1BUF = PROTOCOL_I2C_WHO();
                    break;
                case SEN_I2C:
                    pdata = PROTOCOL_I2C_SEN();
                    SSP1BUF = *pdata;
                    break;
                case GET_I2C:
                    SSP1BUF = PROTOCOL_I2C_GET();
                    break;
            }
            break;

        case I2C_SLAVE_READ_COMPLETED:
            break;
        default:
            break;

    }
}
