#!/usr/bin/env python

import paho.mqtt.client as client
import yaml
import json

def on_message(client, userdata, msg):
    global csv_file
    print(str(msg.payload))
    try:
        payload = json.loads(msg.payload)
        timestamp = payload['timestamp']
        device_id = payload['device_id']
        data = payload['data']
        cpe = payload['cpe']
        location = payload['location']
        csv_file.write('{},{},{},{},{}\n'.format(timestamp, location, cpe, device_id, data))
        csv_file.flush()
    except Exception as e:
        print(e)

if __name__ == '__main__':

    with open('./agent.yaml', 'r') as f:
        conf = yaml.load(f)
        mqtt = conf['mqtt']
        topic = mqtt['topic']

    with open('./capture.csv', 'w') as csv_file:
        csv_file.write('timestamp,location,cpe,device_id,data\n')
        csv_file.flush()

        client = client.Client()
        client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
        client.subscribe(topic)
        client.on_message = on_message
        client.loop_forever()
