# Mini PLC for smart home and smart office

This is a project to archive building blocks for mini PLC.

## Interfaces among building blocks

All the blocks developed in this project support [Plug&Play protocol](./doc/PROTOCOL.md):
- "CLI over UART"
- "CLI over UART" proxy for I2C backplane

#### "CLI over UART"

This construct targets a small router (or CPE) supporting PLC capabilities:

![arch](https://docs.google.com/drawings/d/16cHL6QpvqUBJZJr4kIXCOJ5CkqHTnKV7QXeDOKtFB80/pub?w=640&h=480)

I use [FTDI](http://www.ftdichip.com/)'s USB-UART bridges that support [VCP (Virtual COM Port)](http://www.ftdichip.com/Drivers/VCP.htm) so that Windows PC recognizes them as COM ports:

- Linux automatically load a driver for FTDI chip and recognizes it as "/dev/ttyUSB\*" device.
- Windows PC automatically download [the FTDI driver for Windows](http://www.ftdichip.com/Drivers/VCP.htm) from the Internet (or you need to install it manually) for the first time, then recognizes it as COM\* device.
- In case of OpenWrt, you need to install [the FTDI driver for OpenWrt](https://wiki.openwrt.org/doc/hardware/port.serial) manually by using opkg package manager.

#### "CLI over UART" proxy for I2C backplane

This construct is for [DIN housing for RasPi](https://www.phoenixcontact.com/online/portal/gb?1dmy&urile=wcm:path:/gben/web/offcontext/insite_landing_pages/efd3ca11-79cd-4971-a56c-3c20e29139ad/efd3ca11-79cd-4971-a56c-3c20e29139ad) and DIN rail.

![arch2](https://docs.google.com/drawings/d/1PbKogx1trD3deEXCsBjhFhOSxsouPb-ITNwMbOb6nc0/pub?w=640&h=480)

## PIC16F1 MCU models

The following MCUs are used in this project:

|Model     |# of pins |Characteristics                 |
|----------|-----|--------------------------------|
|[PIC16F1825](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|14   |Variety of Serial communications|
|[PIC16F1829](http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)|20   |Variety of Serial communications|
|[PIC16F1829LIN](http://www.microchip.com/wwwproducts/en/PIC16F1829LIN)|20|MCU + Vreg + LIN tranceiver (SiP)|

![pic16f1](./doc/starting_project.png)

## Communication modules for UART

- [UART-USB converter with 5V supply]
- [UART-USB converter with 3.3V supply](./doc/STEP_DOWN.md)
- [UART-WiFi with ESP8266(ESP-WROOM-02)]
- [UART-RS232 converter](https://www.amazon.co.jp/NulSom-Inc-NS-RS232-02-H-%E6%A5%B5%E5%B0%8FRS232-TTL%E3%82%B3%E3%83%B3%E3%83%90%E3%83%BC%E3%82%BF%E3%83%A2%E3%82%B8%E3%83%A5%E3%83%BC%E3%83%AB-%EF%BC%A4%E3%82%B5%E3%83%96%EF%BC%99%E3%83%94%E3%83%B3%E3%82%AA%E3%82%B9%E3%82%B3%E3%83%8D%E3%82%AF%E3%82%BF%EF%BC%88%E3%82%B1%E3%83%BC%E3%82%B9%E3%82%AD%E3%83%83%E3%83%88%E4%BB%98%E3%81%8D%EF%BC%89/dp/B00OPWLXDW/ref=sr_1_8?ie=UTF8&qid=1491089182&sr=8-8&keywords=rs232+TTL)
- [EnOcean tranceiver](https://www.enocean.com/jp/enocean_modules_928mhz/tcm-410j/)

## Base boards

#### PIC16F1825

The base board board described here is used as USB/UART devices.

![prototype2](./doc/prototype2.jpg)

- 51k ohm pull-up register sits between Vdd and MCLR pins
- The 5P pin socket is for PICkit3.
- The green jumper pin is to enable/disable the LED blinking.
- The tactile switch is a reset button: shorts MCLR pin to GND.

The following is schematic of the base board prototype #2:

![pico](https://docs.google.com/drawings/d/1PItJDNvJnGcRv9vkCc_wwkTdFGRrPGMQLLfpC9JUxE8/pub?w=680&h=400)

|Jumper pin|on           |off         |
|----------|-------------|------------|
|J1        |Enable LED   |Disable LED |
|J2        |Tactile SW as reset button|Tactile SW as general purpose SW|

The base board will also supports I2C bus network topology:

![i2c](https://docs.google.com/drawings/d/1LMcj8u0Y6h_CqZZ0nOh6kb68Wq6j4hkFFoqb6wR4EJw/pub?w=680&h=382)


![backplane_test](./doc/backplane_test.png)

#### PIC16F1829/PIC16F1829LIN

The base board board described here is used as building blocks of mini PLC.

![pico2](https://docs.google.com/drawings/d/1_WCC4vuPbIT2im9c337ibk5xEq9WKzrT9907IOWTCCA/pub?w=680&h=400)

## Implementation

Note: I use [MPLAB Code Configurator (MCC)](http://www.microchip.com/mplab/mplab-code-configurator) to generate code for USART, I2C, PWM, Timer etc.

#### Plug&Play protocol

- [Plug&play protocol specification](./doc/PROTOCOL.md)
- [Implementation](./pic16f1825/lib/protocol.X)

Including it as a library:
- [Step1: include the protocol library directory](./doc/mcc_eusart4.png)
- [Step2: include the protocol library in your project](./doc/mcc_eusart3.png)
- [Step3: exclude mcc generated eusart libraries from your project](./doc/mcc_eusart2.png)
- [Step4: enable eusart interrupts](./doc/mcc_eusart.png)

#### USB/UART devices with PIC16F1825

=> [USAGE](./doc/USAGE.md)

- [5V: Just red LED & tactile switch (Omron B3J-1000 or small one)]
- [5V: Solenoid (SparkFun ROB11015)]
- [5V: Character LCD actuator block](./pic16f1825/character_lcd.X), [pin assignment](./doc/lcd_pin.png)
- [5V: Distance sensor block (HC-SR04)](./pic16f1825/distance.X), [pin assignment](./doc/distance_pin.png)
- [5V: Acceleration sensor block （KXR94-2050)](./pic16f1825/acceleration.X), [pin assignment](./doc/acceleration_pin.png)
- [5V: Speed sensor block (A1324LUA-T)](./pic16f1825/rotation.X), [pin assignment](./doc/rotation_pin.png)
- [5V: Servo motor actuator block (TowerPro sg90)](./pic16f1825/servomotor.X), [pin assignment](./doc/servomotor_pin2.png), [duty at 3%](./doc/duty@3.BMP), [duty at 12%](./doc/duty@12.BMP)

#### UVC (USB Video Class)
- [USB video cam with OpenCV](./camera)

#### HID (Human Interface Devices)
- [Barcode scanner (OSOYOO 4209)]

## Use cases

#### Working with Python

=> [IoT gateway implementation](./agent)

#### Working with Juputer/IPython

I connected the acceleration sensor block to my laptop PC on which Node-RED was running. Then I performed actual measurement with the sensor on a bus. All the data was saved on MongoDB via Node-RED, and I dumped it on a file in CSV format.

- [Data](./jupyter/accelerometer.csv)
- [Jupyter notebook](./jupyter/bus.ipynb)

#### Working with Node-RED

It is pretty easy to integrate these blocks with Node-RED. I have run Node-RED on my RasPi 3 as well as on Windows PC:
- [node-red-1](./doc/node-red-1.png)
- [node-red-2](./doc/node-red-2.png)

#### Using the blocks with OpenWrt

==> [OpenWrt](./openwrt)

I run OpenWrt on my router (Buffalo BHR-4GRV) that I bought in Akihabara, Tokyo. The router was really cheap and the price was around $30.

The router is also equipped with one USB port, thus my router works as IoT gateway for the blocks developed in this project.

![bhr-4grv](./doc/bhr-4grv.png)

#### Internet of Hamsters

See this [video](https://youtu.be/PpgvDuwcBxU) on YouTube.

I used [this small magnet for healthcare](http://www.elekiban.com/products/).

## Managing the system with Ansible

==> [Ansible](./ansible)

I have been using Ansible to manage the system:
- sensor/actuator blocks
- IoT gateways (RasPi/OpenWrt)

## Security issues

JTAG and UART...

## IoT building blocks in the real world

#### DIN-rail housings

- [Phoenix Contact BC moduler housings](https://www.phoenixcontact.com/online/portal/us?1dmy&urile=wcm:path:/usen/web/main/products/subcategory_pages/modular_housing_bc_p-01-04-03/3a371abc-34e5-438f-a723-d2ab8e20eaa1)

#### EnOcean modules

- [EnOcean modules for Japanese market](https://www.enocean.com/jp/enocean_modules_928mhz/)

#### Industry 4.0

There is a standard protocol ["IO-Link"](http://www.io-link.com/en/Technology/what_is_IO-Link.php?thisID=73) standardized by IEC. The goal of IO-Link is similar to my goal, except that my project targets RasPi and small routers/OpenWrt as a PLC-like controller for hobby-use sensor/actuator components.

- [I/O-Link download site](http://www.io-link.com/en/Download/Download.php)

#### In-vehicle networking

LIN is designed to work with cheap 8bit MCUs:
- [LIN](https://en.wikipedia.org/wiki/Local_Interconnect_Network)

#### Smart home / HEMS

- [DLNA](https://www.dlna.org/) and [DTCP-IP](https://en.wikipedia.org/wiki/Digital_Transmission_Content_Protection)
- [ECHONET](https://echonet.jp/english/)
- [Home controller(Panasonic)](http://www2.panasonic.biz/es/densetsu/ha/mansion_ha/syohin/pvjcontroller/)

#### Smart office / BEMS

- [BACnet](https://www.priceindustries.com/content/uploads/assets/literature/service-installation-manuals/section%20e1/bacnet_wiring_guidelines.pdf#search=%27BACNET+MS%2FTP%27)
- [Yanzi Networks](https://yanzi.se/index.jsp)

#### Controllers

- [Siemens SIMATIC IOT2020](http://w3.siemens.com/mcms/pc-based-automation/en/industrial-iot/pages/default.aspx)
- [Phoenix RPI-BC](https://www.phoenixcontact.com/online/portal/gb?1dmy&urile=wcm:path:/gben/web/offcontext/insite_landing_pages/efd3ca11-79cd-4971-a56c-3c20e29139ad/efd3ca11-79cd-4971-a56c-3c20e29139ad)

#### IoT frameworks

- [OpenFog](https://www.openfogconsortium.org/)
- [Fuse](https://medium.com/@gigastacey/dell-plans-an-open-source-iot-stack-3dde43f24feb#.ggg76e529)
- [OpenWrt](http://events.linuxfoundation.org/sites/events/files/slides/Intelligent%20IoT%20Gateway%20on%20OpenWrt.pdf#search=%27OpenWrt+IoT%27)
- [Sakura IoT](https://iot.sakura.ad.jp/)
