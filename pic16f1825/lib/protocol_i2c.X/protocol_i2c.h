#ifndef PROTOCOL_I2C_H
#define	PROTOCOL_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
   
    /*
     * Basic commands
     */
    #define WHO2 0x01
    #define SCN2 0x02
    #define SAV2 0x03
    #define SEN2 0x06
    #define SET2 0x07
    #define GET2 0x08
    
    /*
     * Functions
     */
    void PROTOCOL_I2C_Initialize(uint8_t device_id);
    uint8_t PROTOCOL_I2C_Who(void);
    uint8_t PROTOCOL_I2C_Scn(void);
    void PROTOCOL_I2C_Sav(void);
    uint8_t PROTOCOL_I2C_Sen(uint8_t *data);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_I2C_H */

