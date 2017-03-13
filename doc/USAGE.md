## Sensor blocks (compliant to the Plug&Play protocol)

|Sensor      |Device ID   |Set range         |Meaning             | Set actual value     |Data          |
|------------|------------|------------------|--------------------|----------------------|--------------|
|Distance    |HC-SR4      |10 ~ 255          |Measurement period  |0.1sec ~ 2.55sec      |Distance in cm|
|Rotation    |A1324LUA-T  |0(\*1) or 1 ~ 255 |Measurement period  |0 or 1sec ~ 255sec    |rpm           |

(\*1) In case of "0", it outputs "1" at every revolution.

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
