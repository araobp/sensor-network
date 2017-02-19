# Plug&play protocol

## WHO (who are you?)
```
   PIC                GW
    |<------WHO--------|
    |                  |
    |---<device_id>--->|
```

## INT (set interval) for sensors
```
   PIC                GW
    |<----INT:<num>----|
```

## SAV (save the current setting onto EEPROM)
```
   PIC                GW
    |<------SAV--------|
```

The number is saved in PIC'S EEPROM.

## STA (start sending data)
```
   PIC                GW
    |<------STA--------|
```

## STP (stop sending data)
```
   PIC                GW
    |<------STP--------|
```

## SET (set the new setting to the device and also save the new setting onto EEPRON
```
   PIC                GW
    |<----SET:<num>----|
```

## GET (return the current setting)
```
    PIC                GW
     |<------GET--------|
```
