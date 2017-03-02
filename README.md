# Blocks: pretty-easy IoT with PIC16F1

![accelrerometer](./doc/accelerometer.png)

## Background and motivation

GPIO, I2C, SPI and UART/USART are widely adopted by sensor/actuator components. However, IT guys are not familiar with those low-level interfaces, so I have decided to develop a gateway that works as a bridge between those low-level interfaces and serial (UART/USB).

I use Microchip PIC16F1 series 8bit MCU to develop the gateway, since they are very cheap and small.

## IoT building blocks

IoT is Internet of Things. Sensors/actuators communicate with other sensors/actuators or with human being.

Most of sensors/actuators require some sort of gateway to be able to speak IP. They call it IoT gateway.

I focus on opening up sensors/actuators in a LEGO-block manner. In this project, I develop "open" sensors/actuators following the architecture below:

```

<--- IoT low-level block ---->
sensor
actuator      MCU     Comm. module    IoT gateway
  +---+      +---+      +---+         +--------+
  |   |      |   |      |   |         | Win PC |           (         )
  |   |--*1--|   |--*2--|   |---USB---| RasPi  |----------(The internet)
  |   |      |   |      |   |         | OpenWrt|           (         )
  +---+      +---+      +---+         +--------+

*1 GPIO/I2C/SPI/UART
*2 UART
```
I mainly use Microchip PIC16F1 series 14 pins MCU to develop the blocks.

## USB/UART (i.e., serial) as universal interface for IoT blocks

All the blocks support USB/UART interface. I define two types of bridges to connect the blocks to the Internet.

I use [FTDI](http://www.ftdichip.com/)'s USB-UART bridge. Linux automatically load a driver for FTDI chip and recognizes it as "/dev/ttyUSB\*" device. On the other hand, Windows PC automatically download the FTDI driver from the Internet (or you need to install it manually), then recognizes it as COM* device. In case of OpenWrt, you need to install the FTDI driver manually by using opkg package manager.

IoT blocks with FTDI's USB-UART bridge(*1):
```
                                   +-----+                      
[sensor block 1]-UART-[*1]---USB---|     |                    (        )
[sensor block 2]-UART-[*1]---USB---| USB |---[IoT gateway]---( Internet )
[actuator block 1]-UART-[*1]--USB--| hub |    such as RasPi   (        )
[actuator block 2]-UART-[*1]--USB--|     |    or OpenWrt
                                   +-----+
```

Note: In case of PIC16F1455/1459, USB-UART bridge is unnecessary.

IoT blocks with MQTT-UART(or "UART over IP"?) bridge:
```
                   MQTT-UART bridge
                      +-------+                      (        )
[sensor block 3]-UART-|Comm.  |---wireless access---( Internet )
                      |module |                      (      )            
                      +-------+
```

The communication module supports one of these: WiFi, LTE, 5G, Wi-SUN, LoRaWAN, Sigfox.

## PIC16F1 MCU models

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

## Base board prototyping for IoT building blocks

#### Base board prototype #2

This prototype uses PIC16F1825. It costs around $3, much cheaper than A*duino, but it requires one hour for soldering components onto the universal board. I have made three boards so far (it means I spent three hours to make them).

![prototype2](./doc/prototype2.jpg)

- 51k ohm pull-up register sits between Vdd and MCLR pins
- The 5P pin socket is for PICkit3.
- The green jumper pin is to enable/disable the LED blinking.
- The tactile switch is a reset button: shorts MCLR pin to GND.

#### Schematic of the base board

The following is schematic of the base board prototype #2:

![pico](https://docs.google.com/drawings/d/1PItJDNvJnGcRv9vkCc_wwkTdFGRrPGMQLLfpC9JUxE8/pub?w=680&h=400)

|Jumper pin|on           |off         |
|----------|-------------|------------|
|J1        |Enable LED   |Disable LED |
|J2        |Tactile SW as reset button|Tactile SW as general purpose SW|

The blocks may use I2C for inter-block communications.  In that case, the master needs to hide all the details of low-level communications from the user.

![pico_i2c](https://docs.google.com/drawings/d/1LMcj8u0Y6h_CqZZ0nOh6kb68Wq6j4hkFFoqb6wR4EJw/pub?w=480&h=283)

## Firmware implementation

Note: I use [MPLAB Code Configurator (MCC)](http://www.microchip.com/mplab/mplab-code-configurator) to generate code for USART, I2C, PWM, Timer etc.

#### PIC16F1455

- [I2C-USB bridge]

#### PIC16F1825

- [5V: Just red LED & tactile switch (Omron B3J-1000 or small one)]
- [5V: Solenoid (SparkFun ROB11015)]
- [5V: Distance sensor block (HC-SR04)](./distance.X)
 - [pin assignment](./doc/distance_pin.png)
- [5V: Acceleration sensor block （KXR94-2050)](./acceleration.X)
 - [pin assignment](./doc/acceleration_pin.png)
- [5V: Speed sensor block (A1324LUA-T)](./rotation.X)
 - [pin assignment](./doc/rotation_pin.png)

#### PIC16F1829

- [3.3V: Orientation sensor block (HMC5883L and L3GD20)](./orientation.X)
 - [pin assignment](./doc/orientation_pin.png)
- [5V: Servo motor actuator block (TowerPro sg90)](./servomotor.X)
 - [pin assignment](./doc/servomotor_pin.png)

Note: calibrating HMC5883L is a little hard. I read the data sheet that shows how to calibrate it.

## Non-PIC blocks

- [USB video with OpenCV](./camera)
- [Text-to-Speech for Japanese]

## Sensor/actuator boards

I have made these [boards](./doc/BOARD.md) so far. They are connected to the base board.

## Plug&play protocol

- [Plug&play protocol specification](./doc/PROTOCOL.md)
- [Implementation](./protocol.X)

## Networking with the blocks

I plan to develop "UART router" and "routing protocol over UART" supporting various networking topology.

For example, hub-and-spoke topo:
```             
                  +----+
[block]-- UART ---|    |
                  |PIC |--- USB ---[host]
[block]-- UART ---|    |
                  +----+
```

## Use cases

#### Working with Juputer/IPython

I connected the acceleration sensor block to my laptop PC on which Node-RED was running. Then I performed actual measurement with the sensor on a bus. All the data was saved on MongoDB via Node-RED, and I dumped it on a file in CSV format.

- [Data](./jupyter/accelerometer.csv)
- [Jupyter notebook](./jupyter/bus.ipynb)

I notice that Physics is in the heart of IoT:
- F = ma
- v = v0 + at

#### Working with Node-RED

I have heavily used Node-RED in the path month, and I think I would use Node-RED just for testing my sensors quickly.

All the blocks just use USB as an interface to IoT gateway, so it is pretty easy to integrate these blocks with Node-RED.

I have run Node-RED on my RasPi 3:
- [node-red-1](./doc/node-red-1.png)
- [node-red-2](./doc/node-red-2.png)

#### Using the blocks with UNIX pipe

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

#### Using the blocks with OpenWrt

==> [OpenWrt](./openwrt)

I run OpenWrt on my router (Buffalo BHR-4GRV) that I bought in Akihabara, Tokyo. The router was really cheap and the price was around $30.

The router is also equipped with one USB port, thus my router works as IoT gateway for the blocks developed in this project.

![bhr-4grv](./doc/bhr-4grv.png)

So I don't need expensive IoT gateway products (Intel ATOM-based or ARM-based ones). I don't even want RasPi in most of cases.

OpenWrt is free OS for IoT gateways as well as for WiFi routers.

For example, I connect the acceleration sensor block to my router, then I think that it may work as a distributed earthquake sensing system.

#### Internet of Hamsters

If you want to learn IoT by doing, you had better have hamster. There are a lot of "things" you want to work on with IoT.

==> [HAMSTER.md](./doc/HAMSTER.md)

## Managing the system with Ansible

==> [Ansible](./ansible)

I am going to use Ansible to manage the system:
- sensor/actuator blocks
- IoT gateways (RasPi/OpenWrt)
- Node-RED or other applications

I tried out Ansible in the past (three years ago). It was not a good tool, but it seems to me that a lot of improvements have been made since then. So I use Ansible this time.
