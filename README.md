# IoT low level blocks

# Background and motivation

I use PIC to develop sensors/actuators for IoT.

This repo has low level blocks for IoT:

```
                           +-----+                      
[sensor block 1]---USB-----|     |                      (        )
[sensor block 2]---USB-----| USB |----[IoT gateway]----(  Cloud   )
[actuator block 1]---USB---| hub |                      (        )
[actuator block 2]---USB---|     |
                           +-----+
```

Note: I use MPLAB Code Configurator (MCC) to generate code for USART, I2C etc.

## Blocks

- [Orientation sensor block](./orientation.X)
