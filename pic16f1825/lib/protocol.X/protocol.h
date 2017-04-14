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
     * I2C backplane slave address
     */
    uint8_t CLI_SLAVE_ADDRESS = 0x00;
    
    /*
     * Functions
     */
    void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_hanldler, void *set_handler);
    void PROTOCOL_Loop();
    void PROTOCOL_Set_Func(void *loop_func);
    void PROTOCOL_Set_Extension_Handler(void *extension_handler);
    void PROTOCOL_Write_Device_Address(uint8_t device_id_i2c);
    uint8_t PROTOCOL_Read_Device_Address(void);
    void PROTOCOL_Call_Start_Handler(void);
    void PROTOCOL_Call_Stop_Handler(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

