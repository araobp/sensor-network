#ifndef PROTOCOL_I2C_H
#define	PROTOCOL_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
       
    /*
     * Basic commands
     */
    #define WHO2 0x01
    #define SAV2 0x03
    #define SEN2 0x06
    #define SET2 0x07
    #define GET2 0x08
    
    /*
     * Functions
     */
    void PROTOCOL_I2C_Initialize(uint8_t device_id);
    uint8_t PROTOCOL_I2C_Who(void);
    void PROTOCOL_I2C_Sav(void);
    void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer);
    uint8_t* PROTOCOL_I2C_Sen();
    
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_I2C_H */

