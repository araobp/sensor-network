# Pubsub

## How it works

MULTI_A1324LUA_T runs in pubsub mode, so every SEN request results in type "NO_DATA".

When the sensor block has detected magenetic field change, the block calls "PROTOCOL_I2C_Send_uint8_t(6, position);" to send the change to the master via I2C (or outputs it to UART).

```
#RSC
$:RSC:21,0,0,0|0,0,0,0|0,0,0,0|0,0,0,0|0,0,0,0|19,0,0,0|0,0,0,0
#STA
%19:FLOAT:0.81,0.24,0.54
%19:FLOAT:0.83,0.24,0.54
%21:UINT8_T:0,0,0,0,1,0   ==> Close
%19:FLOAT:0.81,0.24,0.53
%19:FLOAT:0.84,0.23,0.51
%19:FLOAT:0.83,0.22,0.50
%19:FLOAT:0.84,0.24,0.50
%19:FLOAT:0.85,0.23,0.53
%21:UINT8_T:0,0,0,0,0,0   ==> Far
%19:FLOAT:0.82,0.25,0.54
%19:FLOAT:0.80,0.24,0.54
          :
```
