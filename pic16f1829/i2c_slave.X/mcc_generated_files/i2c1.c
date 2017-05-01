#include "i2c1.h"
#include "../protocol.h"

#define I2C_SLAVE_ADDRESS 0x01 
#define I2C_SLAVE_MASK    0x7F

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
void I2C1_StatusCallback(I2C1_SLAVE_DRIVER_STATUS i2c_bus_state);

void I2C1_Initialize(void)
{
    // initialize the hardware
    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP Standard Speed; UA dontupdate; CKE disabled; D_nA lastbyte_address; 
    SSP1STAT = 0x80;
    // SSPEN enabled; WCOL no_collision; CKP disabled; SSPM 7 Bit Polling; SSPOV no_overflow; 
    SSP1CON1 = 0x26;
    // ACKEN disabled; GCEN enabled; PEN disabled; ACKDT acknowledge; RSEN disabled; RCEN disabled; ACKSTAT received; SEN disabled; 
    SSP1CON2 = 0x80;
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    // SSPMSK 127; 
    SSP1MSK = (I2C_SLAVE_MASK << 1);  // adjust UI mask for R/nW bit            
    // SSPADD 8; 
    SSP1ADD = (I2C_SLAVE_ADDRESS << 1);  // adjust UI address for R/nW bit

    // clear the slave interrupt flag
    PIR1bits.SSP1IF = 0;
    // enable the master interrupt
    PIE1bits.SSP1IE = 1;

}

void I2C1_ISR ( void )
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
            I2C1_StatusCallback(I2C1_SLAVE_READ_COMPLETED);
        }
        else
        {
            // callback routine should write data into SSPBUF
            I2C1_StatusCallback(I2C1_SLAVE_READ_REQUEST);
        }
    }
    else if(0 == SSP1STATbits.D_nA)
    {
        // this is an I2C address

        if(0x00 == i2c_data)
        {
            // this is the General Call address
            I2C1_StatusCallback(I2C1_SLAVE_GENERAL_CALL_REQUEST);
        }
        else
        {
            // callback routine should prepare to receive data from the master
            I2C1_StatusCallback(I2C1_SLAVE_WRITE_REQUEST);
        }
    }
    else
    {
        I2C_slaveWriteData   = i2c_data;

        // callback routine should process I2C_slaveWriteData from the master
        I2C1_StatusCallback(I2C1_SLAVE_WRITE_COMPLETED);
    }

    SSP1CON1bits.CKP    = 1;    // release SCL

}


void I2C1_StatusCallback(I2C1_SLAVE_DRIVER_STATUS i2c_bus_state)
{

    static uint8_t slaveWriteType   = SLAVE_NORMAL_DATA;
    static uint8_t prevData = 0xff;
    uint8_t *pdata;
    switch (i2c_bus_state)
    {
        case I2C1_SLAVE_WRITE_REQUEST:
            // the master will be sending the eeprom address next
            slaveWriteType  = SLAVE_DATA_ADDRESS;
            break;

        case I2C1_SLAVE_GENERAL_CALL_REQUEST:
            // the master will be sending general call data next
            slaveWriteType  = SLAVE_GENERAL_CALL;
            break;

        case I2C1_SLAVE_WRITE_COMPLETED:

            switch(slaveWriteType)
            {
                case SLAVE_DATA_ADDRESS:
                    if (prevData == SET_I2C) {
                        PROTOCOL_SET(I2C_slaveWriteData);
                    } else {
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
                        }
                    }
                    prevData = I2C_slaveWriteData;
                    break;

                case SLAVE_GENERAL_CALL:
                    if (I2C_slaveWriteData == PLG_I2C) SSP1CON2bits.GCEN = 0;  // Disable General Call reception
                    break;

                case SLAVE_NORMAL_DATA:
                    break;
                default:
                    break;

            }

            slaveWriteType  = SLAVE_NORMAL_DATA;
            break;

        case I2C1_SLAVE_READ_REQUEST:
            
            switch (I2C_slaveWriteData)
            {
                case WHO_I2C:
                    SSP1BUF = PROTOCOL_I2C_WHO();
                    break;
                case STS_I2C:
                    if (PROTOCOL_I2C_TLV_Status()) SSP1BUF = STS_SEN_READY;
                    else SSP1BUF = STS_NO_DATA;
                    break;
                case SEN_I2C:
                    pdata = PROTOCOL_I2C_SEN();
                    if (pdata != NULL) {
                        SSP1BUF = *pdata;
                    } else {
                        SSP1BUF = 0xff;
                    }
                    break;
                case GET_I2C:
                    SSP1BUF = PROTOCOL_I2C_GET();
                    break;
            }
            break;

        case I2C1_SLAVE_READ_COMPLETED:
            break;
        default:
            break;

    }

}
