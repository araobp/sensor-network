# Test sequence

Date: 2017/04/17

## Preparation

#### Write I2C slave address to Slave 1 to 3
```
  --+------+-------+-------+-- I2C
    |      |       |       |
   [S3]   [S2]    [S1]    [M]
    |      |       |
    |      |       +--UART/USB--[H1]
    |      +----------UART/USB--[H2]
    +-----------------UART/USB--[H3]       
```
```
H1: WHO
S1: $:WHO:TEMPLATE_PIC16F1829
H1: WDA:16
H1: RDA
S1: $:RDA:16
```
```
H2: WHO
S2: $:WHO:TEMPLATE_PIC16F1829
H2: WDA:17
H2: RDA
S2: $:RDA:17
```
```
H3: WHO
S3: $:WHO:TEMPLATE_PIC16F1829
H3: WDA:18
H3: RDA
S3: $:RDA:18
```

## Test sequence
```
  --+------+-------+-------+-- I2C
    |      |       |       |
   [S3]   [S2]    [S1]    [M]
　        　　　   　       |
      　  　        　　　　+--UART/USB--[H]
   I2C slave address
   S1: 16
   S2: 17
   S3: 18
```
```
H: STP
H: *:STP:ACK
H: WJO
H: $:WHO:BACKPLANE_MASTER
H: MAP
H: $:MAP:16,17,18
H: I2C:16
H: WHO
H: $:WHO:16
H: GET
H: $:GET:200
H: I2C:17
H: WHO
H: $:WHO:17
H: GET
H: $:GET:200
H: I2C:18
H: $:WHO:18
H: GET
H: $:GET:200
H: I2C:1
H: STA
H: %18:0,1,2,3,255
H: %16:0,1,2,3,255
H: %17:0,1,2,3,255
H: %18:-128,-10,0,10,127
H: %16:-128,-10,0,10,127
H: %17:-128,-10,0,10,127
H: %18:65535,3840,256,255,1
H: %16:65535,3840,256,255,1
H: %17:65535,3840,256,255,1
H: %18:-32768,-1000,0,1000,32767
H: %16:-32768,-1000,0,1000,32767
H: %17:-32768,-1000,0,1000,32767
H: %18:-327.68,-1.99,-1.01,0.00,1.01,1.99,327.67
H: %16:-327.68,-1.99,-1.01,0.00,1.01,1.99,327.67
H: %17:-327.68,-1.99,-1.01,0.00,1.01,1.99,327.67
H: STP
H: *:STP:ACK
```
