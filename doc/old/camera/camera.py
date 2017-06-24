import cv2
import numpy as np
import os
import yaml
import paho.mqtt.client as mqtt
import base64
import traceback

BGR2GRAY = cv2.COLOR_BGR2GRAY
BGR2HSV = cv2.COLOR_BGR2HSV

conf_file = open('./conf.yaml')
conf = yaml.load(conf_file)

DEVICE_ID = conf['device_id']
VIDEO_ID = conf['video_id']
FORMAT = conf['format']
FILTERS = conf['filters']
MQTT = conf['mqtt']

# OpenCV: resize
def resize(img, args):
    param = args.split('x')
    width = int(param[0])
    height = int(param[1])
    return cv2.resize(img, (width, height))

# OpenCV: convert color spalce
def color(img, args):
    return cv2.cvtColor(img, globals()[args])

# capture, filter, then publish
def capture():
    cap = cv2.VideoCapture(VIDEO_ID)
    ret,img = cap.read()
    img = cv2.convertScaleAbs(img)
    try:
        for filter_ in FILTERS:
            for func in filter_.keys():
                args = filter_[func]
                img = globals()[func](img,args)
        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 100]
        result,encimg = cv2.imencode('.jpg', img, encode_param)
        client.publish('image', base64.b64encode(encimg), qos=0, retain=False)
    except Exception:
        traceback.print_exc()

# MQTT client connected
def on_connect(client, userdata, flag, rc):
    client.subscribe(DEVICE_ID)

# MQTT message received
def on_message(client, usedata, msg):
    cmd = msg.payload
    print('cmd: {}'.format(cmd))
    if (cmd == 'capture'):
        capture()

if __name__ == '__main__':
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT['host'], MQTT['port'], 60)
    client.loop_forever()

