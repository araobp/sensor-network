#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol_i2c.h"

uint8_t device_id_;

// initialization
void PROTOCOL_I2C_Initialize(uint8_t device_id) {
    device_id_ = device_id;
}

uint8_t PROTOCOL_I2C_Who(void) {
    return device_id_;
}

uint8_t PROTOCOL_I2C_Scn(void) {
    return device_id_;
}

void PROTOCOL_I2C_Sav(void) {
    
}