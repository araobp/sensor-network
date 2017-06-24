#!/usr/bin/env python

import paho.mqtt.client as client
import yaml

TOPIC = 'sensor'

def on_message(client, userdata, msg):
    print(str(msg.payload))

if __name__ == '__main__':

    f = open('./agent.yaml', 'r')

    conf = yaml.load(f)
    mqtt = conf['mqtt']
    topic = mqtt['topic']

    client = client.Client()
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
    client.subscribe(topic)
    client.on_message = on_message
    client.loop_forever()
