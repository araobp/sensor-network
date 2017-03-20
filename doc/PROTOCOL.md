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

 Slave             master
   |                  |
   |<------STP--------|
   |                  |
   |<------WHO--------|
   |----DEVICE_ID---->|
   |                  |
   |<----SET:<num>----|
   |                  |
   |<-----SAV---------|
   |                  |
   |<-----STA---------|
   |                  |
