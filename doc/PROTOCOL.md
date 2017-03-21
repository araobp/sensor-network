# Plug&play protocol (UART)

This is a very simple I/O link supporting Plug&Play. This protocol works over UART。

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
    |                  |
    |-------ACK------->|
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

## Recommended link start-up sequence

```
 slave             master
   |                  |
   |<------STP--------|  Send STP(Stop) to the slave to stop sending data.
   |-------ACK------->|  Confirm that ACK has been received on the master side.
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
