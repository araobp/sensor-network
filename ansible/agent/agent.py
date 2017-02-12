#!/usr/bin/env python

import paho.mqtt.client as client
from time import sleep
import yaml
import serial
import os
import json

# MQTT topic
TOPIC = 'sensor'

if __name__ == '__main__':

    f = open('./agent.yaml', 'r')

    conf = yaml.load(f)
    cpe = conf['cpe']
    location = conf['location']
    mqtt = conf['mqtt']
    topic = mqtt['topic']

    client = client.Client()
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)

    # FTDI driver
    ftdi_list = os.listdir('/dev/serial/by-id')

    dev_list = []
    for ftdi in ftdi_list:
        path = '/dev/serial/by-id/{}'.format(ftdi)
        tty = os.path.realpath(path)  # symbolic link
        ser = serial.Serial(tty)
        usb = tty.split('/')[2]
        dev_list.append((ftdi, usb, ser))

    while True:
        for dev in dev_list:
            usb = dev[1]
            ser = dev[2]
            raw_data = ser.readline()[:-1]
            data = json.dumps(dict(cpe=cpe, location=location, usb=usb, data=raw_data))
            client.publish(topic, data)

