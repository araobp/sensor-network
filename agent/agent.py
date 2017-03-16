#!/usr/bin/env python

import paho.mqtt.client as client
from time import sleep, time
import yaml
import serial
import os
import json

PERIOD = 0.01
MAX = 1000
AGENT_RUNNING = 'agent'

dev_list = {} 

def on_message(client, userdata, message):
    global dev_list
    topic = message.topic
    payload = message.payload
    print(topic)
    print(payload)
    print(dev_list[topic])

if __name__ == '__main__':
    
    f = open('./agent.yaml', 'r')

    conf = yaml.load(f)
    cpe = conf['cpe']
    location = conf['location']
    mqtt = conf['mqtt']
    topic = mqtt['topic']
    settings = conf['settings']

    client = client.Client()
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
    client.on_message = on_message

    # FTDI driver
    ftdi_list = os.listdir('/dev/serial/by-id')

    for ftdi in ftdi_list:
        path = '/dev/serial/by-id/{}'.format(ftdi)
        tty = os.path.realpath(path)  # symbolic link
        ser = serial.Serial(tty)

        # Protocol operation
        ser.write('STP\n')
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.write('WHO\n')
        device_id = ser.readline()[:-1]
        if device_id in settings:
            for cmd in settings[device_id]:
                ser.write(cmd+'\n')
            client.subscribe(device_id)
        ser.write('STA\n')
        print('device detected: {}'.format(device_id))

        usb = tty.split('/')[2]
        dev_list[device_id] = dict(ftdi=ftdi, usb=usb, ser=ser)

    client.loop_start()

    cnt = 0

    while True:
        sleep(PERIOD)
        for device_id, dev in dev_list.iteritems():
            usb = dev['usb']
            ser = dev['ser']
            if ser.in_waiting > 0:
                raw_data = ser.readline()[:-1]
                data = dict(timestamp='{0:.2f}'.format(time()),
                            device_id=device_id,
                            cpe=cpe,
                            location=location,
                            usb=usb,
                            data=raw_data)
                client.publish(topic, json.dumps(data))
            else:
                if cnt > MAX:
                    client.publish(AGENT_RUNNING, 'agent running')
                    cnt = 0
                else:
                    cnt += 1

