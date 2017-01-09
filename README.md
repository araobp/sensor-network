# IoT low level blocks

![hc_sr04_test](./doc/hc_sr04_test.png)

## Background and motivation

I use PIC to develop sensors/actuators for IoT.

This repo has low level blocks for IoT:

```
                            DC 5V
                              |
                              V
                           +-----+                      
[sensor block 1]---USB-----|     |                      (        )
[sensor block 2]---USB-----| USB |----[IoT gateway]----(  Cloud   )
[actuator block 1]---USB---| hub |    such as RasPi     (        )
[actuator block 2]---USB---|     |
                           +-----+
```

Note: I use MPLAB Code Configurator (MCC) to generate code for USART, I2C etc.

## Blocks

- [Distance sensor block (HC-SR04)]
- [Orientation sensor block (HMC5883L and L3GD20)](./orientation.X)
- [Servo motor actuator block (TowerPro sg90)]
- [USB-UART bridge with 5V supply]
- [USB-UART bridge with 3.3V supply](./doc/STEP_DOWN.md)

## PIC16F models

I mainly use PIC16F1825.

|Model     |pins |Characteristics                          |
|----------|-----|-----------------------------------------|
|PIC16F1508|20   |High-resolution PWM with CLCs            |
|PIC16F1509|20   |High-resolution PWM with CLCs, larger memory size|
|PIC16F1823|14   |Serial communications                    |
|[PIC16F1825](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|14   |Serial communications, larger memory size|
|[PIC16F1829](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|20   |Serial communications, larger memory size|

## Working with Node-RED

All the blocks just use USB as an interface to IoT gateway, so it is very easy to integrate these blocks with Node-RED.

I run Node-RED on my RasPi 3:

![node-red-1](./doc/node-red-1.png)

![node-red-2](./doc/node-red-2.png)
