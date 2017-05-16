#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c2_util.h"

#define _XTAL_FREQ 500000

/* TEXAS INSTRUMENTS HDC1000
 * http://www.ti.com/lit/ds/symlink/hdc1000.pdf#search=%27HDC1000+datasheet%27
 */
#define DEVICE_ID "HDC1000"
#define HDC1000_ADDR 0x0040
#define TEMPERATURE 0x00
#define HUMIDITY 0x01
#define CONFIGURATION 0x02
#define CONFIG_DATA 0x00

void init(void) {
    __delay_ms(30);  // wait for 15msec
    i2c2_write(HDC1000_ADDR, CONFIGURATION, CONFIG_DATA);
}

void inv_handler(void) {
    uint8_t measure[4];  // temperature MSB/LSB, humidity MSB/LSB
    int8_t data[2];
    int32_t t;
    uint8_t status;
    
    LATCbits.LATC7 ^= 1;

    // Temperature measurement
    status = i2c2_write_no_data(HDC1000_ADDR, TEMPERATURE);
    __delay_ms(10);
    status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[0], 2);  // MSB, LSB
    t = (int32_t)((uint16_t)measure[0] * 256 + (uint16_t)measure[1]);
    data[0] = (int8_t)(t * 165 / 65536 - 40);  // temperature

    __delay_ms(5);

    // Humidity measurement
    status = i2c2_write_no_data(HDC1000_ADDR, HUMIDITY);
    __delay_ms(10);
    status = i2c2_read_no_reg_addr(HDC1000_ADDR, &measure[2], 2);  // MSB, LSB 
    t = (int32_t)((uint16_t)measure[2] * 256 + (uint16_t)measure[3]);
    data[1] = (int8_t)(t * 100 /65536);  // humidity

    // output the result
    PROTOCOL_I2C_Send_int8_t(2, data);
}

void main(void)
{            
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    WDT_Initialize();

    // Protocol initialization
    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, inv_handler, 20);
    
    // Enable interrupt
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // USART initialization
    EUSART_Initialize();

    // Device initialization
    I2C2_Initialize();
    init();

    // I2C backplane initialization
    I2C1_Initialize();  // Enable I2C backplane

    // Infinite loop
    PROTOCOL_Loop();
}