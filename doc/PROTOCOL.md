# Plug&play protocol (UART/I2C)

This is a very simple I/O link supporting Plug&Play. This protocol works over UART or I2C at a minimal cost. I am also considering to support it over LIN.

## Common operations among the blocks

#### WHO 0x01 (who are you?)
```
  slave              master
    |<------WHO--------|
    |                  |
    |---<device_id>--->|
```

#### SCN 0x02 (scan new device / I2C only)

This request is sent as I2C General Call to all the slaves on the I2C bus.
```
  slave              master
    |<------SCN--------|
    |                  |
    |---<device_id>--->|
```

#### SAV 0x03 (save the current setting onto EEPROM)
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

#### SEN 0x06 (request sensor data/ I2C only)
```
 slave              master
   |<------SEN--------|
   |                  |
   |------<num>------>|
```

#### SET 0x07 (set the new setting to the device)
```
  slave              master
    |<----SET:<num>----|
```

#### GET 0x08 (return the current setting)
```
 slave              master
   |<------GET--------|
   |                  |
   |------<num>------>|
```

## Recommended link start-up sequence

```
 slave             master
   |                  |
   |<------STP--------|  Send STP(Stop) to the slave to stop sending data.
   |-------ACK------->|  Confirm that ACK has been received on the master side.
   |                  |  Clear the output buffer on the master side.
   |                  |  Clear the input buffer on the master side.
   |<------WHO--------|  Send WHO(Who are you?) to get device ID of the block.
   |----DEVICE_ID---->|  Keep the device ID on the master side.
   |                  |
   |<----SET:<num>----|  Send SET to configure the device.
   |                  |
   |<-----SAV---------|  Send SAV to save the config on the slave side.
   |                  |
   |<-----STA---------|  Send STA(Start) to make the device start sending data.
   |                  |
```

## Block-specific operations

### Character LCD block

#### INI 0x20 (initialize LCD)
```
PIC                GW
 |<------INI--------|
```

#### CMD 0x21 (command)
```
PIC                GW
 |<----CMD:<cmd>----|
```
"cmd" is one-byte command in decimal.

#### DAT 0x22 (data)
```
PIC                GW
 |<---DAT:<data>----|
```
"data" is one-byte data in decimal.

#### CLR 0x23 (clear)
```
PIC                GW
 |<------CLR--------|
```

#### STR 0x24 (string)
```
PIC                GW
 |<--STR:<string>---|
```
"string" is string data in ASCII code.

#### CUL 0x25 (move cursur left)
```
PIC                GW
 |<------CUL--------|
```

#### CUR 0x26 (move cursor right)
```
PIC                GW
 |<------CUR--------|
```

#### NWL 0x27 (new line)
```
PIC                GW
 |<------NWL--------|
```

#### HOM 0x28 (return home)
```
PIC                GW
 |<------HOM--------|
```
