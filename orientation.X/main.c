#include <math.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart.h"

#define _XTAL_FREQ 500000

// HMC5883L
#define HMC5883L_ADDRESS 0x1e
#define X_MSB_REG 0x03
#define X_LSB_REG 0x04
#define Z_MSB_REG 0x05
#define Z_LSB_REG 0x06
#define Y_MSB_REG 0x07
#define Y_LSB_REG 0x08
#define MODE_REGISTER 0x02
//#define CONTINUOUS_MEASUREMENT 0x00
#define SINGLE_MEASUREMENT 0x01
// Calibration parameters determined by actual measurement following the data sheet
#define X_MAX 263
#define X_MIN -326
#define Y_MAX 55
#define Y_MIN -537

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

// I2C write
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

// I2C read
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

// convert LSB and MSB into int16_t
int16_t convert(uint8_t msb, uint8_t lsb) {
    return (int16_t)(((msb << 8) & 0xff00) | lsb);
}

float Xsf=1.0;
float Ysf=1.0;
float Xoff=0.0;
float Yoff=0.0;

// HMC5883L calibration
void calibrate_hmc5883l(int16_t x_max, int16_t x_min, int16_t y_max, int16_t y_min) {
    int16_t x_sf = (y_max - y_min) / (x_max - x_min);
    if (x_sf > 1.0) {
        Xsf = x_sf;
    }
    int16_t y_sf = (x_max - x_min) / (y_max - y_min);
    if (y_sf > 1.0) {
        Ysf = x_sf;
    }
    Xoff = ((x_max - x_min) / 2 - x_max) * Xsf;
    Yoff = ((y_max - y_min) / 2 - y_max) * Ysf;
}

// orientation in radian
double get_radian(void)
{
    uint8_t buf[1];
    uint8_t lsb, msb;
    int16_t x, y, z;
    
    // read registers
    i2c_read(HMC5883L_ADDRESS, X_MSB_REG, buf, 1);
    msb = buf[0];
    i2c_read(HMC5883L_ADDRESS, X_LSB_REG, buf, 1);
    lsb = buf[0];
    x = convert(msb, lsb);
    
    i2c_read(HMC5883L_ADDRESS, Y_MSB_REG, buf, 1);
    msb = buf[0];
    i2c_read(HMC5883L_ADDRESS, Y_LSB_REG, buf, 1);
    lsb = buf[0];
    y = convert(msb, lsb);
    
    /*
    i2c_read(HMC5883L_ADDRESS, Z_MSB_REG, buf, 1);
     msb = buf[0];
    i2c_read(HMC5883L_ADDRESS, Z_LSB_REG, buf, 1);
    lsb = buf[0];
    z = (int16_t)(convert(msb, lsb) * SENSIVITY);
    */
    
    // calibration (assuming that the sensor is level with the ground),
    // i.e., XH is x and YH is y.
    x = (int16_t)(Xsf * x + Xoff);
    y = (int16_t)(Ysf * y + Yoff);
  
    // return radian
    return atan2((double)y, (double)x);
}

// orientation in degree
int16_t get_degree() {
  return (int16_t)(get_radian() * 180 / M_PI);
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

    calibrate_hmc5883l(X_MAX, X_MIN, Y_MAX, Y_MIN);
    
    // printf("Init completed\n");
    
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
                
        int16_t orientation = get_degree();
        i2c_write(HMC5883L_ADDRESS, MODE_REGISTER, SINGLE_MEASUREMENT);

        i2c_read(L3GD20_ADDRESS, OUT_X_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_X_H, buf, 1);
        msb = buf[0];
        x_rate = (int16_t)(convert(msb, lsb) * SENSIVITY);
        i2c_read(L3GD20_ADDRESS, OUT_Y_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_Y_H, buf, 1);
        msb = buf[0];
        y_rate = (int16_t)(convert(msb, lsb) * SENSIVITY);
        i2c_read(L3GD20_ADDRESS, OUT_Z_L, buf, 1);
        lsb = buf[0];
        i2c_read(L3GD20_ADDRESS, OUT_Z_H, buf, 1);
        msb = buf[0];
        z_rate = (int16_t)(convert(msb, lsb) * SENSIVITY);
        
        // output data in JSON format
        printf("{\"orientation\": %d, \"x-axis\": %d, \"y-axis\": %d, \"z-axis\": %d}\n", orientation, x_rate, y_rate, z_rate);
 
        // blink LED
        LATBbits.LATB7 ^= 1;
        
        // Clear Watch Dog Timer
        CLRWDT();
    }
}