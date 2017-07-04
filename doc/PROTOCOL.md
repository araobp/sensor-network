# Plug&play protocol (UART/I2C)

Update: 2017-JUN-28

This is a very simple I/O link supporting Plug&Play. This protocol works over UART and I2C at a minimal cost. I am also considering to support it over CAN.

## Common operations among the blocks

Legend
W: I2C Write
R: I2C Read
G: I2C General Call

#### WHO (R)0x01 (who are you?)
```
  slave              master
    |<------WHO--------|
    |                  |
    |---<device_id>--->|
```

#### PLG (G)0x02 (Plug / I2C only, General Call)
```
  slave              master
    |<------PLG--------|
    |----(I2C ACK)---->|
```

#### SAV (W)0x03 (save the current setting onto EEPROM)
```
  slave              master
    |<------SAV--------|
```

The current setting is saved in PIC'S EEPROM.

#### STA (W)0x04 (start sending data / UART only)
```
  slave              master
    |<------STA--------|
```

#### STP (W)0x05 (stop sending data / UART only)
```
  slave              master
    |<------STP--------|
    |                  |
    |-------ACK------->|
```

#### INV (W)0x06 (invoke sensor data collection / I2C only)
```
  slave              master
    |<------INV--------|
```

#### SEN (R)0x07 (request sensor data/ I2C only)
```
 slave              master
   |<------SEN--------|
   |                  |
   |-----<value>----->|
```

#### SET (W)0x08 (set the new setting to the device)
```
  slave              master
    |<---SET:<value>---|
```

#### GET (R)0x08 (return the current setting)
```
 slave              master
   |<------GET--------|
   |                  |
   |-----<value>----->|
```

#### WDA (Write I2C slave device address / UART only)
```
slave              master
  |<------EXT--------|
```

#### EXT (W)0x10 (send extended command / I2C only)
```
slave              master
  |<----WDA:<id>-----|
```

#### RDA (Read I2C slave device address / UART only)
```
slave              master
  |<------RDA--------|
  |                  |
  |-------<id>------>|
```

## Recommended link start-up sequence

```
 slave                master
   |                    |
   |<-------STP---------|  Send STP(Stop) to the slave to stop sending data.
   |-----*:STP:ACK----->|  Confirm that ACK has been received on the master side.
   |                    |  Clear the output buffer on the master side.
   |                    |  Clear the input buffer on the master side.
   |<------WHO----------|  Send WHO(Who are you?) to get device ID of the block.
   |-$:WHO:<device ID>->|  Keep the device ID on the master side.
   |                    |
   |<----SET:<value>----|  Send SET to configure the device.
   |                    |
   |<------SAV----------|  Send SAV to save the config on the slave side.
   |                    |
   |<------STA----------|  Send STA(Start) to make the device start sending data.
   |                    |
```

## TLV format of SEN response payload

I2C backplane master sends SEN to I2C backplane slave to fetch sensor data. The data follows this format, and MSB is sent before LSB:

```
+------+------+-------------------+
|Type  |Length|Value              |
|8bit  |8bit  |8bit * length      |
+------+------+-------------------+
```
Type is one of the following data types:
- uint8_t: 0 ~ 255
- int8_t: -128 ~ 127
- uint16_t: 0 ~ 65535
- int16_t: -32768 ~ 32767
- (int16_t)(float * 100): -327.68 ~ 327.67

In case of uint16_t, int16_t and float, Value contains data in this form:
```
+------+------+----------+----------+----+
|Type  |Length|[MSB][LSB]|[MSB][LSB]|... |
+------+------+----------+----------+----+
```

## TLV format of EXT_I2C payload

I2C backplane master sends EXT_I2C to I2C backplane slave to send a char array of extended command. The message follows this format:

```
+-------+------+---------------------------+
|Type   |Length|Value                      |
|EXT_I2C|8bit  |Char array ending with '\0'|
+-------+------+---------------------------+
```

Note that Length is the number of characters including '\0'.

## Block-specific operations

### Backplane master (scheduler)

#### PLG (detect I2C backplane slave plug-in)

This operation is performed in background at every T_PLG timer interval.
```
new backplane slave            backplane master
(dev addr 0x0n)
    |<----General Call PLG------------|
    |-----(I2C ACK bit on)----------->|
    |                                 | start device scan process
      |<----WHO to dev addr 0x01------|
        |<----WHO to dev addr 0x02----|
    |<----WHO to dev addr 0x0n--------|
    |-----<device ID>---------------->| detect new backplane slave
```

#### I2C (set I2C backplane slave address)
address = 1 means I2C backplane Master.

```
  slave                        master
(backplane master)
    |<--I2C:<address in decimal>--|
```

#### MAP (show device map)
```
  slave                                      master
(backplane master)
    |<-----------------MAP---------------------|
    |                                          |
    |--$:MAP:<list of slave addresses in CSV-->|

    *1  $:MAP:<list of slave addresses in CSV>
```

#### % (sensor data)
```
slave                                        master
(backplane master)
  |---%<address>:<type:<data array in CSV>---->|
```

Example:
```
%10:FLOAT:0.09,-0.13,+1.06
```

### Character LCD block

#### INI (initialize LCD)
```
 slave              master
   |<------INI--------|
```

#### CMD (command)
```
 slave              master
   |<----CMD:<cmd>----|
```
"cmd" is one-byte command in decimal.

#### DAT (data)
```
 slave              master
   |<---DAT:<data>----|
```
"data" is one-byte data in decimal.

#### CLR (clear)
```
 slave              master
   |<------CLR--------|
```

#### STR (string)
```
 slave              master
   |<--STR:<string>---|
```
"string" is string data in ASCII code.

#### CUL (move cursur left)
```
 slave              master
   |<------CUL--------|
```

#### CUR (move cursor right)
```
 slave              master
   |<------CUR--------|
```

#### NWL (new line)
```
 slave              master
   |<------NWL--------|
```

#### HOM (return home)
```
 slave              master
   |<------HOM--------|
```

## EEPROM area

|Address|Usage                 |
|-------|----------------------|
|0      |Device ID             |
|1      |Device setting        |

## I2C address registry

|Address |Device ID              |Capability                     |FOSC  |UART baud rate|
|--------|-----------------------|-------------------------------|------|--------------|
|0x01(01)|BACKPLANE-MASTER       |I2C master (w/ 8msec timer)    |32MHz |115200        |
|0x10(16)|AQM1602XA-RN-GBW       |Character LCD                  |      |9600          |
|0x11(17)|A1324LUA-T             |Hall sensor                    |      |9600          |
|0x12(18)|HDC1000                |Temperature and humidity sensor|      |9600          |
|0x13(19)|KXR94-2050             |Three-axis accelerometer       |32MHz |9600          |
