#ifndef PROTOCOL_H
#define	PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif
   
    /*
     * Basic commands
     */
    #define WHO "WHO"
    #define INT "INT"
    #define SAV "SAV"
    #define STA "STA"
    #define STP "STP"
    #define SET "SET"
    #define GET "GET"
    #define ACK "ACK"
    #define WDA "WDA"
    #define RDA "RDA"

    /*
     * EEPROM area
     */
    #define DEVICE_ID_I2C_ADDRESS 0
    #define DEVICE_SETTING_ADDRESS 1
    
    /*
     * Functions
     */
    void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_hanldler, void *set_handler);
    void PROTOCOL_Loop();
    void PROTOCOL_Set_Func(void *loop_func);
    void PROTOCOL_Set_Extension_Handler(void *extension_handler);
    void PROTOCOL_Write_Device_Address(uint8_t device_id_i2c);
    uint8_t PROTOCOL_Read_Device_Address(void);

    // backplane-slave may call these functions
    void PROTOCOL_STA(void);
    void PROTOCOL_STP(void);
    void PROTOCOL_SET(uint8_t value);
    uint8_t PROTOCOL_GET(void);
    void PROTOCOL_SAV(void);

    // backplane-slave-specific functions
    void PROTOCOL_I2C_Initialize(uint8_t device_id);
    uint8_t PROTOCOL_I2C_WHO(void);
    void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer);
    bool PROTOCOL_I2C_TLV_Status(void);
    uint8_t* PROTOCOL_I2C_Sen();

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
    #define STS_SEN_READY 0x01

    /*
     * I2C backplane general call address
     */
    #define GENERAL_CALL_ADDRESS 0x00

    /*
     * Timers
     */
    #define T_PLG 255  // Plug check timer : 255 * 10msec = 2.55sec

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
    uint8_t CLI_SLAVE_ADDRESS = 0x00;  // means backplane master 
    #define TEMPLATE_I2C 0x10
    #define HC_SR04_I2C 0x11
    #define A1324LUA_T_I2C 0x12
    #define KXR94_2050_I2C 0x13
    #define TOWER_PRO_SG90_I2C 0x14
    #define AQM1602XA_RN_GBW_I2C 0x15
    #define HDC1000_I2C 0x16
 
    /*
     * TLV
     * 
     * Note: TYPE_FLOAT data contains (int16_t)(float data * 100)
     */
    #define TYPE_UINT8_T 0x01
    #define TYPE_UINT16_T 0x02
    #define TYPE_INT8_T 0x03
    #define TYPE_INT16_T 0x04
    #define TYPE_FLOAT_T 0x05
    
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

