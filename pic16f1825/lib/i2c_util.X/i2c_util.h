#ifndef I2C_UTIL_H
#define	I2C_UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

    // I2C write
    uint8_t i2c_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data);
    uint8_t i2c_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_UTIL_H */

