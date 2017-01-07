
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart.h"

#define _XTAL_FREQ 500000

// L3GD20 gyroscope
#define L3GD20_ADDRESS 0b1101011
#define WHO_AM_I 0x0f
#define WHO_AM_I_RESPONSE 0xd4
#define CTRL_REG1 0x20
#define OUT_TEMP 0x26
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2a
#define OUT_Y_H 0x2b
#define OUT_Z_L 0x2c
#define OUT_Z_H 0x2d
#define ENABLE 0x0f
#define SENSIVITY 0.00875  // as per the data sheet

uint8_t i2c_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data) {

    uint8_t buf[2];
    I2C1_MESSAGE_STATUS status;
    uint8_t write_status;
    buf[0] = reg_addr;
    buf[1] = data;
    I2C1_MasterWrite(buf, 2, dev_addr, &status);
    while (status == I2C1_MESSAGE_PENDING);
    if (status == I2C1_MESSAGE_COMPLETE) {
        write_status = 0;
    } else {
        write_status = 1;
    }
    return write_status;
}

uint8_t i2c_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len) {

    uint8_t reg[1];
    I2C1_MESSAGE_STATUS status;
    uint8_t read_status;
    reg[0] = reg_addr;
    I2C1_MasterWrite(reg, 1, dev_addr, &status);
    while (status == I2C1_MESSAGE_PENDING);
    if (status == I2C1_MESSAGE_COMPLETE) {
        I2C1_MasterRead(pbuf, len, dev_addr, &status); 
        while (status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE) {
            read_status = 0;
        } else {
            read_status = 2;
        }
    } else {
        read_status = 1;
    }
    return read_status;
}

int16_t calc_angular_rate(uint8_t msb, uint8_t lsb) {
    int16_t rate = (int16_t)(((msb << 8) & 0xff00) | lsb);
    return (int16_t)(rate * SENSIVITY);
}

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    I2C1_Initialize();
    EUSART_Initialize();
    
    uint8_t buf[2];

    // printf("Init completed\n");
    
    buf[0] = WHO_AM_I;
    
    uint8_t status;
    status = i2c_read(L3GD20_ADDRESS, WHO_AM_I, buf, 1);
    /*
    if (status == 0 && buf[0] == WHO_AM_I_RESPONSE) {
        printf("WHO_AM_I: OK\n");
    } else {
        printf("WHO_AM_I: NG\n");
    }
    */
    
    status = i2c_write(L3GD20_ADDRESS, CTRL_REG1, ENABLE);
    /*
    switch(status) {
        case 0:
            printf("ENABLE OK\n");
            break;
        default:
            printf("ENABLE NG\n");
            break;
    }
    */

    uint8_t lsb;
    uint8_t msb;
    int16_t x_rate, y_rate, z_rate;
    
    while (1)
    {
        __delay_ms(1000);
        i2c_read(L3GD20_ADDRESS, OUT_X_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_X_H, buf, 1);
        msb = buf[0];
        x_rate = calc_angular_rate(msb, lsb);
        i2c_read(L3GD20_ADDRESS, OUT_Y_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_Y_H, buf, 1);
        msb = buf[0];
        y_rate = calc_angular_rate(msb, lsb);
        i2c_read(L3GD20_ADDRESS, OUT_Z_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_Z_H, buf, 1);
        msb = buf[0];
        z_rate = calc_angular_rate(msb, lsb);
        
        printf("{\"x-axis\": %d, \"y-axis\": %d, \"z-axis\": %d}\n", x_rate, y_rate, z_rate);
        
        LATBbits.LATB7 ^= 1;
        CLRWDT();
    }
}