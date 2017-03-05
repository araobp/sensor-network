# IoT gateway implementation

## Plug&Play protocol support

The IoT gateway implementation (agent.py) uses [my original Plug&Play protocol](../doc/PROTOCOL.md)  to detect devices attached to the box (i.e., WinPC, RasPi or OpenWrt router) with USB cables.

## Python is the best language for IoT gateway prototyping

Although I have used Node-RED for testing the sensor devices I have developed so far, I do not think Node.js is not the right choice for prototyping IoT gateway because of the following reasons: 

- RasPi's main lang is Python.
- OpenWrt supports Python.
- Ansible is written in Python.
- Jupyter is for Python.
- OpenCV provides Python APIs.

My conclusion: Node-RED is good for visualizing data from sensors.

## Capture sample on MQTT topic "sensor"

Network configuration:
```
                           +----------+
                           |          |
 [HC-SR04]-------ttyUSB0---+          |
                           | agent.py +--publish-->[Mosquitto]--> client.py
 [A1324LUA-T]----ttyUSB1---+          |
                           |          |
                           +----------+

```

Device detection (agent.py):
```
pi@raspberrypi:~/blocks/agent $ python agent.py
device detected: HC-SR04
device detected: A1324LUA-T
```

Caputure (client.py):
```
pi@raspberrypi:~/blocks/agent $ python client.py
{"location": "tokyo", "device_id": "HC-SR04", "data": "50", "cpe": "raspi001", "usb": "ttyUSB1"}
{"location": "tokyo", "device_id": "HC-SR04", "data": "49", "cpe": "raspi001", "usb": "ttyUSB1"}
{"location": "tokyo", "device_id": "A1324LUA-T", "data": "1", "cpe": "raspi001", "usb": "ttyUSB0"}
{"location": "tokyo", "device_id": "HC-SR04", "data": "49", "cpe": "raspi001", "usb": "ttyUSB1"}
{"location": "tokyo", "device_id": "HC-SR04", "data": "53", "cpe": "raspi001", "usb": "ttyUSB1"}
{"location": "tokyo", "device_id": "A1324LUA-T", "data": "1", "cpe": "raspi001", "usb": "ttyUSB0"}
```
