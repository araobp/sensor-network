#include "mcc_generated_files/mcc.h"
#include "i2c2_util.h"

#define TIMEOUT 1

// I2C write
uint8_t i2c2_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data) {

    uint8_t buf[2];
    I2C2_MESSAGE_STATUS status;
    uint8_t write_status;
    buf[0] = reg_addr;
    buf[1] = data;
    I2C2_MasterWrite(buf, 2, dev_addr, &status);
    while (status == I2C2_MESSAGE_PENDING);
    if (status == I2C2_MESSAGE_COMPLETE) {
        write_status = 0;
    } else {
        write_status = 1;
    }
    return write_status;
}

// I2C read
uint8_t i2c2_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len) {

    uint8_t reg[1];
    I2C2_MESSAGE_STATUS status;
    uint8_t read_status;
    reg[0] = reg_addr;
    I2C2_MasterWrite(reg, 1, dev_addr, &status);
    while (status == I2C2_MESSAGE_PENDING);
    if (status == I2C2_MESSAGE_COMPLETE) {
        I2C2_MasterRead(pbuf, len, dev_addr, &status); 
        while (status == I2C2_MESSAGE_PENDING);
        if (status == I2C2_MESSAGE_COMPLETE) {
            read_status = 0;
        } else {
            read_status = 2;
        }
    } else {
        read_status = 1;
    }
    return read_status;
}

// I2C write w/o data
uint8_t i2c2_write_no_data(uint16_t dev_addr, uint8_t reg_addr) {

    uint8_t buf[1];
    I2C2_MESSAGE_STATUS status;
    uint8_t write_status = 2;
    buf[0] = reg_addr;
    uint8_t timeout = TIMEOUT;
    while (status != I2C2_MESSAGE_FAIL) {
        I2C2_MasterWrite(buf, 1, dev_addr, &status);
        while (status == I2C2_MESSAGE_PENDING);
        if (status == I2C2_MESSAGE_COMPLETE) {
            write_status = 0;
            break;
        }
        if (--timeout == 0) {
            write_status = 1;
            break;
        }
    }
    return write_status;
}

// I2C read w/o reg_addr
uint8_t i2c2_read_no_reg_addr(uint16_t dev_addr, uint8_t *pbuf, uint8_t len) {

    I2C2_MESSAGE_STATUS status;
    uint8_t read_status = 2;
    uint8_t timeout = TIMEOUT;
    while (status != I2C2_MESSAGE_FAIL) {
        I2C2_MasterRead(pbuf, len, dev_addr, &status); 
        while (status == I2C2_MESSAGE_PENDING);
        if (status == I2C2_MESSAGE_COMPLETE) {
            read_status = 0;
            break;
        }
        if (--timeout == 0) {
            read_status = 1;
            break;
        }
    }
    return read_status;
}
