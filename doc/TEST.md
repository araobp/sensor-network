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
S1: TEMPLATE_DEVICE
H: WDA:16
H: RDA
S1: I2C:16
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
S2: TEMPLATE_DEVICE
H: WDA:17
H: RDA
S2: I2C:17
```

## Test
```
   ---+-------+-------+-- I2C
      |       |       |
     [S2]    [S1]    [M]--UART/USB--[H]
```
```
H: WHO
M: BACKPLANE_MASTER
H: MAP
M: 0,0,3,0,0,0
```
```
H: I2C:17
H: WHO
S2: 17
H: STS
S2: 1
H: SEN
S2: 2,4,52,18,120,86
H: SEN
S2: 3,3,246,0,10
```
```
H: I2C:16
H: WHO
S1: 16
H: STS
S1: 1
H: SEN
S1: 4,4,24,252,232,3
H: STP
S1: ACK
(S1 stops blinking LED)
H: STA
(S1 starts blinking LED)

--- NG ---
H: STS
S1: 1
H: SEN
S1: (no response)
H: STA
S1: (no response)
```
