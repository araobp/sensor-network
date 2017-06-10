#!/usr/bin/env python

import paho.mqtt.client as MQTT 
import yaml
import json
import argparse

CONFIG_FILE = 'babycar.yaml'

def on_message(client, userdata, msg):
    print(str(msg.payload))

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--conf", help="config file name", type=str,
            default="agent.yaml")
    args = parser.parse_args()

    with open('./{}'.format(args.conf), 'r') as f:
        conf = yaml.load(f)
        mqtt = conf['mqtt']
        topic = mqtt['topic']

        client = MQTT.Client(protocol=MQTT.MQTTv31)
        client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
        client.subscribe(topic)
        client.on_message = on_message
        client.loop_forever()

