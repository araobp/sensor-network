# IoT low level blocks

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

## Working with Node-RED

All the blocks just use USB as an interface to IoT gateway, so it is very easy to integrate these blocks with Node-RED.

![node-red-1](./doc/node-red-1.png)

![node-red-2](./doc/node-red-2.png)
