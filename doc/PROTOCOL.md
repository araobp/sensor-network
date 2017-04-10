# Plug&play protocol (UART/I2C)

This is a very simple I/O link supporting Plug&Play. This protocol works over UART or I2C at a minimal cost. I am also considering to support it over LIN.

## Common operations among the blocks

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

#### STA (start sending data / UART only)
```
  slave              master
    |<------STA--------|
```

#### STP (stop sending data / UART only)
```
  slave              master
    |<------STP--------|
    |                  |
    |-------ACK------->|
```

#### SEN (R)0x06 (request sensor data/ I2C only)
```
 slave              master
   |<------SEN--------|
   |                  |
   |-----<value>----->|
```

#### SET (W)0x07 (set the new setting to the device)
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

#### STS (R)0x09 (status check / I2C only)
```
slave              master
  |<------STS--------|
  |                  |
  |----<status>----->|
```

## Recommended link start-up sequence

```
 slave              master
   |                  |
   |<------STP--------|  Send STP(Stop) to the slave to stop sending data.
   |-------ACK------->|  Confirm that ACK has been received on the master side.
   |                  |  Clear the output buffer on the master side.
   |                  |  Clear the input buffer on the master side.
   |<------WHO--------|  Send WHO(Who are you?) to get device ID of the block.
   |----DEVICE_ID---->|  Keep the device ID on the master side.
   |                  |
   |<---SET:<value>---|  Send SET to configure the device.
   |                  |
   |<-----SAV---------|  Send SAV to save the config on the slave side.
   |                  |
   |<-----STA---------|  Send STA(Start) to make the device start sending data.
   |                  |
```

## Block-specific operations

### Backplane Master (for debugging)

#### DEV (set device address)
```
 slave              master
   |<----DEV:<dev>----|
```

#### REG (set register address)
```
slave              master
|<----REG:<reg>----|
```

#### RED (read)
```
slave              master
  |<------RED--------|
  |                  |
  |-----<value>----->|
```

#### WRT (write value)
```
 slave              master
   |-------WRT------->|
```

### Character LCD block

#### INI (W)0x20 (initialize LCD)
```
 slave              master
   |<------INI--------|
```

#### CMD (W)0x21 (command)
```
 slave              master
   |<----CMD:<cmd>----|
```
"cmd" is one-byte command in decimal.

#### DAT (W)0x22 (data)
```
 slave              master
   |<---DAT:<data>----|
```
"data" is one-byte data in decimal.

#### CLR (W)0x23 (clear)
```
 slave              master
   |<------CLR--------|
```

#### STR (W)0x24 (string)
```
 slave              master
   |<--STR:<string>---|
```
"string" is string data in ASCII code.

#### CUL (W)0x25 (move cursur left)
```
 slave              master
   |<------CUL--------|
```

#### CUR (W)0x26 (move cursor right)
```
 slave              master
   |<------CUR--------|
```

#### NWL (W)0x27 (new line)
```
 slave              master
   |<------NWL--------|
```

#### HOM (W)0x28 (return home)
```
 slave              master
   |<------HOM--------|
```
