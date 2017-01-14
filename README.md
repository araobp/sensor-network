# Blocks: pretty-easy IoT

![hc_sr04_test](./doc/hc_sr04_test.png)

## Background and motivation

GPIO, I2C, SPI and UART/USART are widely adopted by sensor/actuator components. However, IT guys are not familiar with those low-level interfaces.

I have decided to develop a gateway that works as a bridge between those low-level interfaces and serial (UART/USB).

## Common architecture

```

<--- IoT low-level block ---->
sensor
actuator      MCU     Comm. module
  +---+      +---+      +---+         +---
  |   |      |   |      |   |         | Win PC
  |   |--*1--|   |--*2--|   |---USB---| RasPi
  |   |      |   |      |   |         | IoT gateway
  +---+      +---+      +---+         +---

*1 GPIO/I2C/SPI/UART
*2 UART

MCU works as such a gateway.
```
I use Microchip PIC16F1 series MCU to develop the block.

## USB/UART (i.e., serial) as universal interface for IoT blocks

All the blocks support USB/UART interface. I use two types of bridges to connect the blocks to the Internet:

IoT blocks with USB-UART bridge(*1):
```
                                   +-----+                      
[sensor block 1]-UART-[*1]---USB---|     |                    (        )
[sensor block 2]-UART-[*1]---USB---| USB |---[IoT gateway]---( Internet )
[actuator block 1]-UART-[*1]--USB--| hub |    such as RasPi   (        )
[actuator block 2]-UART-[*1]--USB--|     |
                                   +-----+
```

Note: In case of PIC16F1455/1459, USB-UART bridge is unnecessary.

IoT blocks with MQTT-UART bridge:
```
                   MQTT-UART bridge
                      +-------+                (        )
[sensor block 3]-UART-|ESP8266|---MQTT/WiFi---( Internet )
                      +-------+                (        )            
```

## PIC16F1 models

|Model     |# of pins |Characteristics                 |
|----------|-----|--------------------------------|
|[PIC16F1455](http://ww1.microchip.com/downloads/en/DeviceDoc/40001639B.pdf)|14   |Built-in USB                    |
|[PIC16F1459](http://ww1.microchip.com/downloads/en/DeviceDoc/40001639B.pdf)|20   |Built-in USB                    |
|[PIC16F1508](http://ww1.microchip.com/downloads/en/DeviceDoc/41609A.pdf)|20   |CLCs                            |
|[PIC16F1509](http://ww1.microchip.com/downloads/en/DeviceDoc/41609A.pdf)|20   |CLCs                            |
|[PIC16F1825](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|14   |Variety of Serial communications|
|[PIC16F1829](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|20   |Variety of Serial communications|

## Communication modules

- [USB-UART bridge with 5V supply]
- [USB-UART bridge with 3.3V supply](./doc/STEP_DOWN.md)
- [ESP8266(ESP-WROOM-02)]
- [USB Micro B connector(for PIC16F1455/1459)](http://akizukidenshi.com/catalog/g/gK-06656/)

## prototype

This prototype uses PIC16F1825:

![prototype1](./doc/prototype1.png)

- 10k ohm pull-up register sits between Vdd and MCLR pins (50k ohm is better)
- The 5P pin header (L type) is for PICkit3.
- The 2P pin header (L type) supplies 5V/3.3V to a sensor/actuator component.
- The other 2P pin header is connected to TX/RX of the USB/UART bridge(FT234X)
- The 6P pin headers are connected to PIC16F1825.
- The LED (w/ 1k ohm register) blinks every while loop:
turn on/off every __delay_ms(period);
- The green jumper pin is to enable/disable the LED blinking.
- The tactile switch is a reset button: shorts MCLR pin to GND.

## Hardware schematic

This PIC board is absolutely cheaper than A*duino.

![pico](https://docs.google.com/drawings/d/1PItJDNvJnGcRv9vkCc_wwkTdFGRrPGMQLLfpC9JUxE8/pub?w=480&h=360)

For inter-block communications, UART TX/RX pins may be used as I2C SDA/SCL. But that makes things difficult, and contradicts the goal of this project.

## Firmware implementation

Note: I use MPLAB Code Configurator (MCC) to generate code for USART, I2C, PWM, Timer etc.

- [Distance sensor block (HC-SR04)](./distance.X)
 - [pin assignment](./doc/distance_pin.png)
- [Orientation sensor block (HMC5883L and L3GD20)](./orientation.X)
 - [pin assignment](./doc/orientation_pin.png)
- [Servo motor actuator block (TowerPro sg90)](./servomotor.X)
 - [pin assignment](./doc/servomotor_pin.png)

Note: calibrating HMC5883L is a little hard. I read the data sheet that shows how to calibrate it.

## Using the blocks with UNIX pipe

It is pretty easy!

Things are recognized as "tty devices" on UNIX/Linux or as "COM ports" on Windows.

Linux also supports "/dev/serial/by-id" that assigns an unique name to each thing.

- Reading data from a sensor and feeding it to other UNIX commands via a pipe:
```
$ cat /dev/serial/by-id/<device_id> | command 1 | command 2 ...
```

- Sending data to a sensor/actuator:
```
$ echo <command> > /dev/serial/by-id/<device_id>
```

See this [usage](./doc/USAGE.md) page.

## Python for IoT agent

I think Python is the best choice for writing IoT agent with those IoT low-level blocks. I am writing a sample IoT agent with the blocks with Python Tornado package.

Python is also a choice for Data Analysis (Jupyter/IPython), Computer Vison (OpenCV) and AI (TensorFlow etc).

I have also developed a remote management framework ["tega"](https://github.com/araobp/tega) for IoT.

In addition to that, I am examining if Ansible can be used for managing the things.

## Node-RED for rapid IoT prototyping

Although I am not so fond of Node.js (I would rather use Python Tornado package for  event-driven programs), Node-RED is thought of as a good tool for rapid prototyping in IoT communities, although I am not so agree with that idea...

All the blocks just use USB as an interface to IoT gateway, so it is very easy to integrate these blocks with Node-RED.

I run Node-RED on my RasPi 3:

![node-red-1](./doc/node-red-1.png)

![node-red-2](./doc/node-red-2.png)
