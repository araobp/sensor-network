#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"

/* Sensirion SHT31-DIS
 * https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf
 */

#define SHT31_DIS_ADDR 0x0045
#define CMD_MSB 0x2c
#define CMD_LSB 0x06

const uint16_t RESOLUTION = 65535;

void inv_handler(void) {
    uint8_t measure[6];  // temperature MSB/LSB & CRC, humidity MSB/LSB & CRC
    int8_t data[2];
    int32_t s_t;
    int32_t s_rh;
    uint8_t status;
    
    LED_RED ^= 1;
    
    // Temperature measurement
    status = i2c2_write(SHT31_DIS_ADDR, CMD_MSB, CMD_LSB);
    status = i2c2_read_no_reg_addr(SHT31_DIS_ADDR, measure, 6);
    s_t = (int32_t)((uint16_t)measure[0] * 256 + (uint16_t)measure[1]);  // msb and lsb
    s_rh = (int32_t)((uint16_t)measure[3] * 256 + (uint16_t)measure[4]); // msb and lsb
    
    data[0] = (int8_t)(s_t * 175 / RESOLUTION - 45);  // temperature
    data[1] = (int8_t)(s_rh * 100 / RESOLUTION);  // relative humidty

    // output the result
    PROTOCOL_I2C_Send_int8_t(2, data);
}

void main(void)
{
    // Protocol initialization
    PROTOCOL_Initialize(SHT31_DIS, NULL, NULL, NULL, inv_handler, 125);

    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();
    TMR0_Initialize();
    
    // Enable interrupt
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // USART initialization
    EUSART_Initialize();

    // Device initialization
    I2C2_Initialize();

    // I2C backplane initialization
    I2C1_Initialize();  // Enable I2C backplane

    // Infinite loop
    PROTOCOL_Loop();
}