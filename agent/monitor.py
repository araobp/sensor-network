#!/usr/bin/env python

import paho.mqtt.client as MQTT 
import yaml
import json

def on_message(client, userdata, msg):
    print(str(msg.payload))

if __name__ == '__main__':

    with open('./agent.yaml', 'r') as f:
        conf = yaml.load(f)
        mqtt = conf['mqtt']
        topic = mqtt['topic']

        client = MQTT.Client(protocol=MQTT.MQTTv31)
        client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
        client.subscribe(topic)
        client.on_message = on_message
        client.loop_forever()

