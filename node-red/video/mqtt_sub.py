import paho.mqtt.client as mqtt

def on_connect(client, userdata, rc):
    client.subscribe("test")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)

client.loop_forever()
