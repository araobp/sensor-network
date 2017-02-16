# Plug&play protocol

## WHO (who are you?)
```
   PIC                GW
    |<------WHO--------|
    |                  |
    |---<device_id>--->|
```
## INT (set interval)
```
   PIC                GW
    |<----INT:<num>----|
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
