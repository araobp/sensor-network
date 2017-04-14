#ifndef PROTOCOL_DEFINITION_H
#define	PROTOCOL_DEFINITION_H

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Common operations
 */
#define WHO_I2C 0x01
#define PLG_I2C 0x02
#define SAV_I2C 0x03
#define STA_I2C 0x04
#define STP_I2C 0x05
#define SEN_I2C 0x06
#define SET_I2C 0x07
#define GET_I2C 0x08
#define STS_I2C 0x09
    
/*
 * I2C backplane slave status
 */
#define STS_JUST_PLUGGED 0x01
#define STS_SEN_READY 0x02
 
/*
 * Device IDs
 */
#define HC_SR04 "HC-SR04"
#define A1324LUA_T "A1324LUA-T"
#define KXR94_2050 "KXR94-2050"
#define TOWER_PRO_SG90 "TOWER-PRO-SG90"
#define AQM1602XA_RN_GBW "AQM1602XA-RN-GBW"
#define HDC1000 "HDC1000"
    
/*
 * Backplane slave I2C addresses
 */
#define HC_SR04_I2C 0x10
#define A1324LUA_T_I2C 0x11
#define KXR94_2050_I2C 0x12
#define TOWER_PRO_SG90_I2C 0x13
#define AQM1602XA_RN_GBW_I2C 0x14
#define HDC1000_I2C 0x15
   
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_DEFINITION_H */

