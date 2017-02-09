#!/usr/bin/env python

import paho.mqtt.client as client

HOST = 'localhost'
PORT = 1883

TOPIC = 'sensor'

def on_message(client, userdata, msg):
    print(str(msg.payload))

if __name__ == '__main__':
    client = client.Client()
    client.connect(host=HOST, port=PORT, keepalive=60)
    client.subscribe(TOPIC)
    client.on_message = on_message
    client.loop_forever()
