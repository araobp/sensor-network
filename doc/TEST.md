# Test sequence

Date: 2017/04/17

## Preparation
```
   ---+-------+-------+-- I2C
      |       |       |
     [S2]    [S1]    [M]
　　　　　       |
      　　　　　　+--UART/USB--[H]
```
```
H: WHO
S1: $:WHO:TEMPLATE_DEVICE
H: WDA:16
H: RDA
S1: $:I2C:16
```

```
   ---+-------+-------+-- I2C
      |       |       |
     [S2]    [S1]    [M]
      |
      +--UART/USB--[H]
```
```
H: WHO
S2: $:WHO:TEMPLATE_DEVICE
H: WDA:17
H: RDA
S2: $:I2C:17
```

## Test
```
   ---+-------+-------+-- I2C
      |       |       |
     [S2]    [S1]    [M]--UART/USB--[H]
```
```
H: WHO
M: *:WHO:BACKPLANE_MASTER
H: MAP
M: $:MAP:16,17
H: STP
M: *:STP:ACK
```
```
H: I2C:17
H: WHO
M: $:WHO:17
H: STA
M: %17:0,1,2,3,255
   %17:-128,-10,0,10,127
   %17:65535,3840,256,255,1
   %17:-32768,-1000,0,1000,32767
   %17:-327.68,-1.99,-1.01,0.00,1.01,1.99,327.67
H: STP
N: *:STP:ACK
H: SET:100
H: GET
M: $:GET:100

```
