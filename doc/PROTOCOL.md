# Plug&play protocol

## UART

#### WHO (who are you?)
```
   PIC                GW
    |<------WHO--------|
    |                  |
    |---<device_id>--->|
```

#### SAV (save the current setting onto EEPROM)
```
   PIC                GW
    |<------SAV--------|
```

The current setting is saved in PIC'S EEPROM.

#### STA (start sending data)
```
   PIC                GW
    |<------STA--------|
```

#### STP (stop sending data)
```
   PIC                GW
    |<------STP--------|
```

#### SET (set the new setting to the device)
```
   PIC                GW
    |<----SET:<num>----|
```

#### GET (return the current setting)
```
  PIC                GW
   |<------GET--------|
```

#### ADR (set I2C slave address)
```
  PIC                GW
   |<----ADR:<num>-----|
```

## I2C

#### WHO (who are you?)

It returns device_id.

- PIC(1): I2C slave address is 1
- PIC(2): I2C slave address is 2

```
  PIC(1)      PIC(2)              PIC(GW)
   |<------READ(1:0)---------------|
   |-------ID--------------------->|
               |<--READ(2:0)-------|
               |---ID------------->|
```

#### SAV (save the current setting onto EEPROM)
```
  PIC(1)      PIC(2)              PIC(GW)
   |<-----WRITE(1:2)+0----------|
               |<--WRITE(2:2)+0-|
```

#### SET (set the new setting to the device)
```
  PIC(1)      PIC(2)              PIC(GW)
   |<-----WRITE(1:1)+data----------|
               |<--WRITE(2:1)+data-|
```

#### GET (return the current setting)
```
  PIC(1)      PIC(2)              PIC(GW)
   |<------READ(1:1)---------------|
   |-------ID--------------------->|
               |<--READ(2:1)-------|
               |---ID------------->|
```

#### DAT (read data)

It returns data in ASCII code: <data_0><data_1>...<data_n>'\n'

```
  PIC(1)      PIC(2)              PIC(GW)
   |<------READ(1:3~)--------------|
   |-------DATA------------------->|
               |<--READ(2:3~)------|
               |---DATA----------->|
```
