/*
 * Servo motor block
 * 
 * Usage:
 * Enter "<angle>\n" to set the angle.
 * Enter "w\n" to save the angle onto EEPROM,
 * 
 * Example:
 * "90\n" sets the sg90's arm to 90 degrees.
 * "w\n" saves the angle onto EEPROM. 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"

#define _XTAL_FREQ 1000000

uint16_t calc_duty(uint16_t angle) {
    // dutyValue: Min 35 - Max 155
    return (uint16_t)(155.0 - angle * 120.0 / 180.0);
}

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    EUSART_Initialize();
    EPWM1_Initialize();

    uint16_t dutyValue;
    uint16_t angle;
    uint8_t c;
    uint8_t buf[32] = {'\0'};
    uint8_t cnt = 0;
    
    // read angle from EEPROM
    angle = (uint16_t)DATAEE_ReadByte(0);
    // printf("read angle from EEPROM: %d\n", angle);
    dutyValue = calc_duty(angle);
    EPWM1_LoadDutyValue(dutyValue);
            
    while (1)
    {
        __delay_ms(500);
        LATCbits.LATC4 ^= 1;

        do {
            c = EUSART_Read();
            if (c == '\n') {
                buf[cnt] = '\0';
                cnt = 0;
                if (strcmp(buf, "w") == 0) {
                    DATAEE_WriteByte(0, (uint8_t)angle);
                    // printf("write the last angle onto EEPROM: %d\n", angle);
                } else {
                    angle = atoi(buf);
                    dutyValue = calc_duty(angle);
                    EPWM1_LoadDutyValue(dutyValue);
                }
            } else {
                buf[cnt++] = c;
            }
        } while (EUSART_DataReady);

        CLRWDT();
    } 
}
