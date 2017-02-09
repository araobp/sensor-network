#!/usr/bin/env python

import paho.mqtt.client as client
from time import sleep
import yaml

HOST = 'localhost'
PORT = 1883

TOPIC = 'sensor'

test_msg = 'Hello World!'

if __name__ == '__main__':
    f = open('./agent.yaml', 'r')
    conf = yaml.load(f)
    mqtt = conf['mqtt']
    client = client.Client()
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
    while True:
        client.publish(TOPIC, test_msg)
        sleep(1)
