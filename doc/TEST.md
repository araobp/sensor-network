# Test sequence

Date: 2017/04/17

## Preparation
```
   ---+-------+-------+-- I2C
      |       |       |
     [S1]    [S2]    [M]
      |
      +--UART/USB--[H]
```
```
H: WHO
S1: TEMPLATE_DEVICE
H: WDA:17
H: RDA
S1: 17
```

## Test
```
   ---+-------+-------+-- I2C
      |       |       |
     [S1]    [S2]    [M]--UART/USB--[H]
```
```
H: WHO
M: BACKUP_MASTER
H: MAP
M: 0,0,3,0,0,0
```
```
H: I2C:17
H: WHO
S1: 17
H: STS
S1: 1
H: SEN
S1: 2,4,52,18,120,86
H: SEN
S1: 3,3,246,0,10
```
```
H: I2C:16
H: WHO
S2: 16
H: STS
S2: 1
H: SEN
S2: 4,4,24,252,232,3
H: STP
S2: ACK
(S2 stops blinking LED)

--- NG ---
H: STS
S2: 1
H: SEN
S2: (no response)
H: STA
S2: (no response)
```
