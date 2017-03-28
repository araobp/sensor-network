#include "mcc_generated_files/mcc.h"
#include "i2c_util.h"

// I2C write
uint8_t i2c_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data) {

    uint8_t buf[2];
    I2C_MESSAGE_STATUS status;
    uint8_t write_status;
    buf[0] = reg_addr;
    buf[1] = data;
    I2C_MasterWrite(buf, 2, dev_addr, &status);
    while (status == I2C_MESSAGE_PENDING);
    if (status == I2C_MESSAGE_COMPLETE) {
        write_status = 0;
    } else {
        write_status = 1;
    }
    return write_status;
}

// I2C read
uint8_t i2c_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len) {

    uint8_t reg[1];
    I2C_MESSAGE_STATUS status;
    uint8_t read_status;
    reg[0] = reg_addr;
    I2C_MasterWrite(reg, 1, dev_addr, &status);
    while (status == I2C_MESSAGE_PENDING);
    if (status == I2C_MESSAGE_COMPLETE) {
        I2C_MasterRead(pbuf, len, dev_addr, &status); 
        while (status == I2C_MESSAGE_PENDING);
        if (status == I2C_MESSAGE_COMPLETE) {
            read_status = 0;
        } else {
            read_status = 2;
        }
    } else {
        read_status = 1;
    }
    return read_status;
}
