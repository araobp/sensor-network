#!/usr/bin/env python

import paho.mqtt.client as MQTT 
from time import sleep, time 
import yaml
import json
import logging

# main
if __name__ == '__main__':

    # read config from agent.yaml
    with open('./agent.yaml', 'r') as f:
        conf = yaml.load(f)
        thing_name = conf['thing_name']
        mode = conf['mode']
        log_level = conf['logging']
        mqtt = conf['mqtt']
        device_id = conf['device_id']

    lcd = device_id['lcd']
    led = device_id['led']

    client = MQTT.Client(protocol=MQTT.MQTTv31)
    
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
    
    logging.basicConfig(level=logging.__dict__[log_level])
    logger = logging.getLogger()

    payload = {'thing_name': thing_name, 'device_id': lcd, 'device_name': 'lcd',
            'command': ['Hello World!', 'Guten Tag!']}
    client.publish(thing_name, json.dumps(payload))

    sleep(1)

    payload = {'thing_name': thing_name, 'device_id': led, 'device_name': 'led',
            'command': 'ON'}
    client.publish(thing_name, json.dumps(payload))

    sleep(5)
    payload = {'thing_name': thing_name, 'device_id': led, 'device_name': 'led',
            'command': 'OFF'}
    client.publish(thing_name, json.dumps(payload))
