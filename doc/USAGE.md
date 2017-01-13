
## Distance

#### Changing the measurement cycle

```
Block <-- "10\n" --- Host
```
"10\n" corresponds to 100msec, and the range is from 10 to 255 (i.e., 100msec to 2.5sec).

The value is stored onto EEPROM.

#### Distance data in centimeter

```
Block --- "35\n" -----> Host
```

## Orientation

## Servomotor

#### Setting the angle in degrees

```
Block <-- "90\n" --- Host
```
"90\n" corresponds to 90 degrees, and the range is from 0 to 180.

#### Setting the last angle as start up angle

```
Block <-- "w\n" --- Host
```
The angle in degrees is stored onto EEPROM.
