# Image capture agent for Node-RED

OpenCV officially supports C++ API and Python API.

This MQTT client (written in Python to utilize OpenCV) works as a bridge between an USB camera and Node-RED.

## Architecture

```
                         ---- camera.py ----
[video0]--/dev/video0-->[OpenCV][MQTT client]--->[MQTT server]
```

## Starting MQTT client for video capturing and image filtering
```
$ ./camera.py
```

## Test

- send "capture" command to the client

```
$ ./mqtt.js pub camera1 capture
```

- receive the captured image

```
$ ./mqtt.js sub image > /tmp/test.b64
```

- Decode the image

```
$ base64 -d /tmp/test.b64 > /tmp/test.jpg
```



