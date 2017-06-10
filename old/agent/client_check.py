#!/usr/bin/env python

import paho.mqtt.client as client
import yaml
import json

TOPIC_AGENT = 'agent'


def on_message(client, userdata, msg):
    print(str(msg.payload))

if __name__ == '__main__':


    with open('./agent.yaml', 'r') as f:
        conf = yaml.load(f)
        mqtt = conf['mqtt']

        client = client.Client()
        client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
        client.subscribe(TOPIC_AGENT)
        client.on_message = on_message
        client.loop_forever()
