# Mini PLC with Microchip PIC16F1

This is a project to develop mini PLC (Programmable Logic Controller) in a building-block manner.　In this project, I use Android smart phone, PC or small router as mini PLC.

![compact2](./doc/compact2.png)

Plug&Play protocol are supported for a master board to recognize capabilities of all its slaves in a plug&play manner.

## Interfaces among building blocks

All the blocks developed in this project support [Plug&Play protocol](./doc/PROTOCOL.md) that runs on UART.

```
                    USB hub
                     +---+
[block A]--UART/USB--|   |
[block B]--UART/USB--|   |--[mini PLC]
[block C]--UART/USB--|   |
                     +---+
                     
               hub&spoke topology
```

It also runs on I2C: [I2C backplane specification](./doc/I2C_BACKPLANE_SPEC.pptx).

```
        <- - - - I2C backplane - - - ->
[block A]---[block B]---[block C]---[Scheduler]--UART/USB--[mini PLC]

            bus topology (daisy-chain)
```

## 8bit MCU

I have concluded that [PIC16F1829](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf) is the best choice for this project.

![pic16f1](./doc/starting_project.png)

## Base board prototype

![prototype3](./doc/prototype3.png)

![pico2](https://docs.google.com/drawings/d/1_WCC4vuPbIT2im9c337ibk5xEq9WKzrT9907IOWTCCA/pub?w=680&h=400)

##### Construct examples

One I2C master and three I2C slaves are connected with each other via backplane bus:

![compact](./doc/compact.png)

A similar construct to the above, but all the boards are connected with each other via daisy-chain:

![daisy_chain](./doc/daisy_chain.png)

To extend the distance of bus signal reachability, use CAN standalone controller (SPI): [MCP2525](http://ww1.microchip.com/downloads/en/DeviceDoc/21801e.pdf).

## Implementation

Note: I use [MPLAB Code Configurator (MCC)](http://www.microchip.com/mplab/mplab-code-configurator) to generate code for USART, I2C, PWM, Timer etc.

#### Plug&Play protocol

- [Plug&play protocol specification](./doc/PROTOCOL.md)
- [Implementation](./mini_plc/lib/protocol.X)

Including it as a library:
- [Step1: include the protocol library directory](./doc/mcc_eusart4.png)
- [Step2: include the protocol library in your project](./doc/mcc_eusart3.png)
- [Step3: exclude mcc generated eusart libraries from your project](./doc/mcc_eusart2.png)
- [Step4: enable eusart interrupts](./doc/mcc_eusart.png)

#### Devices

=> [USAGE](./doc/USAGE.md)

- [5V: Just red LED & tactile switch (Omron B3J-1000 or small one)]
- [5V: Solenoid (SparkFun ROB11015)]
- [5V: Character LCD actuator block](./mini_plc/i2c_slave_lcd.X), [pin assignment](./doc/lcd_pin.png)
- [5V: Distance sensor block (HC-SR04)], [pin assignment](./doc/distance_pin.png)
- [5V: Acceleration sensor block （KXR94-2050)](./mini_plc/i2c_slave_accel.X), [pin assignment](./doc/acceleration_pin.png)
- [5V: Speed sensor block (A1324LUA-T)](./mini_plc/i2c_slave_speed.X), [pin assignment](./doc/rotation_pin.png)
- [5V: Servo motor actuator block (TowerPro sg90)], [pin assignment](./doc/servomotor_pin2.png), [duty at 3%](./doc/duty@3.BMP), [duty at 12%](./doc/duty@12.BMP)
