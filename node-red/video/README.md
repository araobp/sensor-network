# Flow

## Pattern A

```
[Video camera] --> [OpenCV for filtering] --> [mqtt_pub.py] --> [mosquitto] --> Node-RED
```

## Pattern B

```
[Video camera] --> [OpenCV for filtering] --> [Image recognition with TensorFlow] --> [mqtt_pub.py] --> [mosquitto] --> Node-RED
```

