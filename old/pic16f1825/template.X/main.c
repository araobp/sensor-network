#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "protocol_i2c.h"

#define _XTAL_FREQ 500000
#define DEVICE_ID "TEMPLATE_DEVICE"

bool running = true;
uint8_t do_func = 0;
uint8_t period_10 = 0;
uint8_t timer_cnt = 0;

void start_handler(void) {
    running = true;
    //printf("Start Handler called\n");
}

void stop_handler(void) {
    running = false;
    //printf("Stop Handler called\n");
}

void set_handler(uint8_t value) {
    period_10 = value/10;
    //printf("Set Handler called\n");
}

void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = 1;
     }
 }

void loop_func(void) {
    uint8_t pbuf[6];
    uint8_t pbuf_uint8_t[5] = {0, 1, 2, 3, 255};
    int8_t pbuf_int8_t[5] = {-128, -10, 0, 10, 127};
    uint16_t pbuf_uint16_t[5] = {0xffff, 0x0f00, 0x0100, 0x00ff, 0x0001};
    int16_t pbuf_int16_t[5] = {-32768, -1000, 0, 1000, 32767};
    float pbuf_float[7] = {-327.68, -1.99, -1.01, 0.00, 1.01, 1.99, 327.67};
    static uint8_t turn = TYPE_UINT8_T;
    int16_t float100;
    if (do_func) {
        LATCbits.LATC3 ^= 1;
        switch(turn) {
            case TYPE_UINT8_T:
                PROTOCOL_I2C_Send_uint8_t(5, &pbuf_uint8_t[0]);
                turn = TYPE_INT8_T;
                break;
            case TYPE_INT8_T:
                PROTOCOL_I2C_Send_int8_t(5, &pbuf_int8_t[0]);
                turn = TYPE_UINT16_T;
                break;
            case TYPE_UINT16_T:
                PROTOCOL_I2C_Send_uint16_t(5, &pbuf_uint16_t[0]);
                turn = TYPE_INT16_T;
                break;
            case TYPE_INT16_T:
                PROTOCOL_I2C_Send_int16_t(5, &pbuf_int16_t[0]);
                turn = TYPE_FLOAT;
                break;
            case TYPE_FLOAT:
                PROTOCOL_I2C_Send_float(7, &pbuf_float[0]);
                turn = TYPE_UINT8_T;
                break;                
        }
        do_func = 0;
    }
}

/*
 * output max abs(measured value) in the period.
 */
void main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    TMR0_Initialize();
    TMR0_SetInterruptHandler(tmr0_handler);

    EUSART_Initialize();

    PROTOCOL_Initialize(DEVICE_ID, start_handler, stop_handler, set_handler);
    PROTOCOL_I2C_Initialize(PROTOCOL_Read_Device_Address());
    PROTOCOL_Set_Func(loop_func);
    PROTOCOL_Loop();
}