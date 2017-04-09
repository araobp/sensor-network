#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <string.h>
#include "protocol_i2c.h"

uint8_t device_id_;

uint8_t *data;

uint8_t test_data = 0x22; 
        
// initialization
void PROTOCOL_I2C_Initialize(uint8_t device_id) {
    device_id_ = device_id;
}

uint8_t PROTOCOL_I2C_Who(void) {
    return device_id_;
}

void PROTOCOL_I2C_Sav(void) {
    
}

uint8_t PROTOCOL_I2C_Sen() {
    return test_data;
}
