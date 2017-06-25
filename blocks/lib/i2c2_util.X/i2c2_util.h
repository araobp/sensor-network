#ifndef I2C2_UTIL_H
#define	I2C2_UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

    uint8_t i2c2_write(uint16_t dev_addr, uint8_t reg_addr, uint8_t data);
    uint8_t i2c2_read(uint16_t dev_addr, uint8_t reg_addr, uint8_t *pbuf, uint8_t len);
    uint8_t i2c2_write_no_data(uint16_t dev_addr, uint8_t reg_addr);    
    uint8_t i2c2_read_no_reg_addr(uint16_t dev_addr, uint8_t *pbuf, uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C2_UTIL_H */

