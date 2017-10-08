#ifndef PROTOCOL_H
#define	PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>
    #include <stddef.h>
    #include <xc.h>

    #define LED_RED LATCbits.LATRC3
    
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
     * Command parser
     */
    #define parse(cmd_name, buf) !strncmp((cmd_name), (buf), 3)

    /*
     * EEPROM area
     */
    #define DEVICE_ID_I2C_ADDRESS 0
    #define DEVICE_SETTING_ADDRESS 1
    #define SCHEDULE_ADDRESS 2
    
    /*
     * Command buffer size
     */
    #define BUF_SIZE 48
    
    void blink_red(uint8_t times);
    
    /*
     * Functions
     */
    void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_hanldler, void *set_handler, void *inv_handler, uint8_t scaler);
    void PROTOCOL_Loop();
    void PROTOCOL_Set_Func(void *loop_func);
    void PROTOCOL_Set_Extension_Handler(void *extension_handler);
    void PROTOCOL_Set_Inv_Handler(void *inv_handler, uint8_t scaler);
    void PROTOCOL_Set_Tick_Handler(void *tick_handler);
    void PROTOCOL_Write_Device_Address(uint8_t device_id_i2c);
    uint8_t PROTOCOL_Read_Device_Address(void);

    void PROTOCOL_STA(void);
    void PROTOCOL_STP(void);
    void PROTOCOL_SET(uint8_t value);
    void PROTOCOL_SAV(void);
    void PROTOCOL_INV(void);
    void PROTOCOL_RST(void);
    void PROTOCOL_EXT(uint8_t *pbuffer);
    
    /*
     * printf for TLV
     */
    void PROTOCOL_Print_TLV(uint8_t dev_addr, uint8_t type, uint8_t length, uint8_t *pbuffer);
        
    /*
     * I2C backplane slave specific functions
     */
    void PROTOCOL_I2C_Set_TLV(uint8_t type, uint8_t length, uint8_t *pbuffer);
    void PROTOCOL_I2C_Reset_TLV(void);
    bool PROTOCOL_I2C_TLV_Status(void);
    // the following functions are type-dependent version of PROTOCOL_I2C_Set_TLV
    void PROTOCOL_I2C_Send_uint8_t(uint8_t length, uint8_t *pbuffer);
    void PROTOCOL_I2C_Send_int8_t(uint8_t length, int8_t *pbuffer);
    void PROTOCOL_I2C_Send_uint16_t(uint8_t length, uint16_t *pbuffer);
    void PROTOCOL_I2C_Send_int16_t(uint8_t length, int16_t *pbuffer);
    void PROTOCOL_I2C_Send_float(uint8_t length, float *pbuffer);
    void PROTOCOL_Send_uint8_t(uint8_t value);
    void PROTOCOL_Send_int8_t(int8_t value);
    void PROTOCOL_Send_uint16_t(uint16_t value);
    void PROTOCOL_Send_int16_t(int16_t value);
    void PROTOCOL_Send_float(float value);
   
    /*
     * I2C slave code "i2c1.c" calls these APIs
     */
    void PROTOCOL_Backplane_Slave_Enabled(void);
    uint8_t PROTOCOL_I2C_WHO(void);
    uint8_t PROTOCOL_I2C_GET(void);
    uint8_t* PROTOCOL_I2C_SEN();  // retrive data from send buffer
    
    void PROTOCOL_Set_Lock(bool lock);
    bool PROTOCOL_Read_Lock(void);
        
    /*
     * I2C backplane common operations
     */
    #define WHO_I2C 0x01
    #define PLG_I2C 0x02
    #define SAV_I2C 0x03
    #define STA_I2C 0x04
    #define STP_I2C 0x05
    #define INV_I2C 0x06
    #define SEN_I2C 0x07
    #define SET_I2C 0x08
    #define GET_I2C 0x09
    #define EXT_I2C 0x0a
    #define ADT_I2C 0x0b
    #define RST_I2C 0x0c

    /*
     * I2C backplane general call address
     */
    #define GENERAL_CALL_ADDRESS 0x00
    
    /***** DEVICE REGISTRY ****************************************************/

    /*
     * Device IDs
     */
    #define HC_SR04 "HC-SR04"
    #define A1324LUA_T "A1324LUA-T"
    #define KXR94_2050 "KXR94-2050"
    #define TOWER_PRO_SG90 "TOWER-PRO-SG90"
    #define AQM1602XA_RN_GBW "AQM1602XA-RN-GBW"
    #define HDC1000 "HDC1000"
    #define SHT31_DIS "SHT31_DIS"

    /*
     * Backplane slave I2C addresses
     */
    #define BACKPLANE_MASTER_I2C 0x01
    uint8_t BACKPLANE_SLAVE_ADDRESS = BACKPLANE_MASTER_I2C ;  // means backplane master 
    #define AQM1602XA_RN_GBW_I2C 0x10  // Character LCD
    #define A1324LUA_T_I2C 0x11  // Hall sensor
    #define HDC1000_I2C 0x12  // Temperature and humidity sensor
    #define KXR94_2050_I2C 0x13  // 3-axis accelerometer 
    #define SHT31_DIS_I2C 0x14 // Temperature and humidity sensor
    #define MULTI_A1324LUA_T_I2C 0x15  // Hall sensors for position sensing 
     
    /*
     * TLV
     * 
     * Note: TYPE_FLOAT data contains (int16_t)(float data * 100)
     */
    #define TYPE_UINT8_T 0x01
    #define TYPE_INT8_T 0x02
    #define TYPE_UINT16_T 0x03
    #define TYPE_INT16_T 0x04
    #define TYPE_FLOAT 0x05
    #define TYPE_NO_DATA 0x06
   
    /***** I2C backplane slave ************************************************/

    typedef enum
    {
        I2C_SLAVE_WRITE_REQUEST,
        I2C_SLAVE_READ_REQUEST,
        I2C_SLAVE_WRITE_COMPLETED,
        I2C_SLAVE_READ_COMPLETED,
        I2C_SLAVE_GENERAL_CALL_REQUEST,
    } I2C_SLAVE_DRIVER_STATUS;

    #define I2C_SLAVE_DEFAULT_ADDRESS          0x08

    void I2C_Initialize(void);

    void I2C_ISR ( void );

    extern volatile uint8_t    I2C_slaveWriteData;

#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

