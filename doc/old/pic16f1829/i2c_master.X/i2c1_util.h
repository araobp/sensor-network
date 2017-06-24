#ifndef I2C1_UTIL_H
#define	I2C1_UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

    uint8_t i2c1_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data);
    uint8_t i2c1_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len);
    uint8_t i2c1_write_no_data(uint16_t dev_addr, uint8_t reg_addr);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C1_UTIL_H */

