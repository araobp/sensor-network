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
    printf("Start Handler called\n");
}

void stop_handler(void) {
    running = false;
    printf("Stop Handler called\n");
}

void set_handler(uint8_t value) {
    period_10 = value/10;
    printf("Set Handler called\n");
}

void tmr0_handler(void) {
     if (++timer_cnt >= period_10) {
        timer_cnt = 0;
        if (running) do_func = 1;
     }
 }

void loop_func(void) {
    uint8_t pbuf[6];
    uint8_t pbuf_uint8_t[3] = {0x01, 0x02, 0x03};
    int8_t pbuf_int8_t[3] = {-10, 0, 10};
    uint16_t pbuf_uint16_t[2] = {0x1234, 0x5678};
    int16_t pbuf_int16_t[2] = {-1000, 1000};
    float pbuf_float_t[3] = {-1.23, 1.23, 4.56};
    static uint8_t turn = TYPE_UINT8_T;
    int16_t float100;
    if (do_func) {
        //printf("Hello World!\n");
        LATCbits.LATC3 ^= 1;
        switch(turn) {
            case TYPE_UINT8_T:
                PROTOCOL_I2C_Set_TLV(TYPE_UINT8_T, 3, &pbuf_uint8_t[0]);
                turn = TYPE_INT8_T;
                break;
            case TYPE_INT8_T:
                pbuf[0] = (uint8_t)pbuf_int8_t[0];
                pbuf[1] = (uint8_t)pbuf_int8_t[1];
                pbuf[2] = (uint8_t)pbuf_int8_t[2];
                PROTOCOL_I2C_Set_TLV(TYPE_INT8_T, 3, &pbuf[0]);
                turn = TYPE_UINT16_T;
                break;
            case TYPE_UINT16_T:
                pbuf[0] = (uint8_t)(pbuf_uint16_t[0] & 0x00ff);
                pbuf[1] = (uint8_t)(pbuf_uint16_t[0] >> 8);
                pbuf[2] = (uint8_t)(pbuf_uint16_t[1] & 0x00ff);
                pbuf[3] = (uint8_t)(pbuf_uint16_t[1] >> 8);
                PROTOCOL_I2C_Set_TLV(TYPE_UINT16_T, 4, &pbuf[0]);
                turn = TYPE_INT16_T;
                break;
            case TYPE_INT16_T:
                pbuf[0] = (uint8_t)(pbuf_int16_t[0] & 0x00ff);
                pbuf[1] = (uint8_t)((pbuf_int16_t[0] >> 8) & 0x00ff);
                pbuf[2] = (uint8_t)(pbuf_int16_t[1] & 0x00ff);
                pbuf[3] = (uint8_t)((pbuf_int16_t[1] >> 8) & 0x00ff);
                PROTOCOL_I2C_Set_TLV(TYPE_INT16_T, 4, &pbuf[0]);
                turn = TYPE_FLOAT_T;
                break;
            case TYPE_FLOAT_T:
                float100 = (int16_t)(pbuf_float_t[0] * 100);
                pbuf[0] = (uint8_t)(float100 & 0x00ff);
                pbuf[1] = (uint8_t)((float100 >> 8) & 0x00ff);
                float100 =(int16_t) pbuf_float_t[1] * 100;
                pbuf[2] = (uint8_t)(float100 & 0x00ff);
                pbuf[3] = (uint8_t)((float100 >> 8) & 0x00ff);
                PROTOCOL_I2C_Set_TLV(TYPE_FLOAT_T, 4, &pbuf[0]);
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