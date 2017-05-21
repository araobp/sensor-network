#!/usr/bin/env python
#
# Python agent for mini PLC
#

import paho.mqtt.client as MQTT 
from time import sleep, time 
import yaml
import serial
import os
import json
import sys
import logging
import math
import re

# Constants
SLEEP_PERIOD = 0.01  # to sleep for 1msec in the infinit loop
DISP_PERIOD = 8  # period for showing speed on LCD
SKIP_PERIOD = 4  # period for skipping LCD process
RECV_TIMEOUT = 10  # 10 times
MAX = 1000  # send agent running message to TOPIC_AGENT 
TOPIC_AGENT = 'agent'  # to check if this agent is running

# Commands over I2C bus
WHO = "WHO"
INT = "INT"
SAV = "SAV"
STA = "STA"
STP = "STP"
SET = "SET"
GET = "GET"
ACK = "ACK"
MAP = "MAP"
I2C = "I2C"
NO_DATA = "NO_DATA"

# Tx buffer to (slow) PIC16F1 EUSART Rx buffer
txBuf = []
in_loop = False

# Command preemption
skip = 0

# display meter string
def meter(speed, temp):
    return 'DSP:SPEED: {0:2.1f} km/h TEMP: {1:2d} deg C'.format(speed, temp)

# (Physical) container
HID = '/dev/hidraw0' # 1D barcode scanner as HID device
READ_CONTAINER_ID = '000000000000'
CONTAINER_ID = '0000000000000'
read_container_id = False

# GPS-related (NEMA format data)
nema_gprmc = re.compile(r'^\$GPRMC,')

# Car speed
WHEEL_RADIUS = 6.0 / 100.0 / 1000.0  # 6cm
PULSE_PERIOD = 0.5  # 500msec
CIRCUMFERENCE = 2 * math.pi * WHEEL_RADIUS

def pulse2speed(pulse):
    return CIRCUMFERENCE * float(pulse) * 60.0 / PULSE_PERIOD * 60.0

# receive MQTT message
def on_message(client, userdata, message):
    global ser, logger, skip
    topic = message.topic
    payload = message.payload
    logger.info('NodeRED==>topic:{}, message:{}'.format(topic, payload))
    data = json.loads(payload)
    thing_name = data['thing_name']
    device_name = data['device_name']
    msg = data['command']
    logging.info('thing_name: {}, device_name: {}, message: {}'.format(thing_name, device_name, msg))
    if topic == thing_name:
        if device_name == 'lcd':
            s = 'DSP:{0:16s}{1:16s}'.format(msg[0], msg[1]) 
            skip = SKIP_PERIOD
            send(ser, s)
        elif device_name == 'led':
            s = 'LED:{}'.format(msg)
            send(ser, s)

# insert delay for PIC16F1 to read EUSART Rx buffer (1 byte length)
def _send(ser, cmd):
    global txBuf, logger
    cmd = cmd + '\n'
    if in_loop and len(txBuf) == 0:
        txBuf.extend([c for c in cmd])
    else:
        for c in cmd:
            ser.write(c)
            sleep(SLEEP_PERIOD)

# receive message from serial port
def recv(ser):
    global logger
    data = ser.readline()[:-1]
    logger.debug("Scheduler==>{}".format(data))
    return data 

# receive response message from serial port
def recv_resp(ser):
    global logger
    resp = ser.readline()[:-1]
    logger.debug(resp)
    return resp.split(':')

# send message to serial port
def send(ser, cmd):
    global logger
    logger.debug("Scheduler<=={}".format(cmd))
    _send(ser, cmd)

# send message to serial port and receive its response
def send_recv(ser, cmd):
    global logger
    logger.debug("Scheduler<=={}".format(cmd))
    _send(ser, cmd)
    cmd_name = cmd.split(':')[0]
    cnt = RECV_TIMEOUT
    resp = None
    while True:
        resp = recv_resp(ser)
        if resp[0:1] != '#':
            cnt -= 1
            try:
                if resp[1] == cmd_name:
                    return resp 
            except:
                pass
            if cnt <= 0:
                logger.warning("command timeout: {}".format(cmd))
                return resp
        sleep(SLEEP_PERIOD)

# receive message from serial port
def recv_gps(ser):
    global logger
    data = ser.readline()[:-1]
    logger.debug("GPS==>:{}".format(data))
    return data 

# barcode scanner character code - 29
def recv_scanner(ser):
    global logger
    end = False
    buf = []
    while True:
        d = ser.read(8)
        for c in d:
            c = ord(c)
            if c == 40:  # '('
                end = True
            elif c > 0:
                if c == 39: 
                    c -= 10 
                buf.append(str(c - 29))
        if end:
            break
    return ''.join(buf)


# main
if __name__ == '__main__':

    # read config from agent.yaml
    with open('./agent.yaml', 'r') as f:
        conf = yaml.load(f)
        thing_name = conf['thing_name']
        mode = conf['mode']
        log_level = conf['logging']
        mqtt = conf['mqtt']
        topic = mqtt['topic']
        device_id = conf['device_id']
        ftdi_vdp = conf['ftdi_vdp']
        lcd_conf = conf['lcd']

    lcd = device_id['lcd']
    led = device_id['led']
    speed = device_id['speed']
    temp = device_id['temp']
    accel = device_id['accel']
    circurator = device_id['circurator']
    gps = device_id['gps']

    client = MQTT.Client(protocol=MQTT.MQTTv31)
    
    client.connect(host=mqtt['host'], port=mqtt['port'], keepalive=60)
    client.on_message = on_message
    
    logging.basicConfig(level=logging.__dict__[log_level])
    logger = logging.getLogger()
    logger.info('Agent starting...')

    ser = ser_gps = ser_scanner = None

    try:
        port = '/dev/serial/by-id/{}'.format(ftdi_vdp['master'])
        ser = serial.Serial(port)
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        send(ser, 'I2C:1')
        sleep(0.5)
        send(ser, STA)
        sleep(0.5)
        logger.info('I2C backplane connected: {}'.format(port))
    except:
        print(e)
        if mode == 'agent':
            logger.critical('unable to communicate with I2C backplane master')
            sys.exit(1)

    try:
        port = '/dev/serial/by-id/{}'.format(ftdi_vdp['gps'])
        ser_gps = serial.Serial(port)
        ser_gps.reset_input_buffer()
        logger.info('GPS connected: {}'.format(port))
    except:
        if mode == 'agent':
            logger.warning('unable to communicate with GPS device')
        elif mode == 'gps_only':
            logger.fatal('unable to communicate with GPS device')
            sys.exit(1)
    
    if mode == 'scanner_only':
        try:
            ser_scanner = open(HID)
            logger.info('scannner connected')
        except:
            logger.info('unable to communicate with scanner device')
            sys.exit(1)

    # subscribe thing name as topic name
    client.subscribe(thing_name)
    
    # start client loop to receive MQTT message
    client.loop_start()

    cnt_adt = 0
    device_name = "unknown"
    cnt_disp = 0
    speed_ph = 0.0
    temp_ps = 0
    humid_ps = 0

    # device initial config
    if ser:
        # Character LCD
        send(ser, 'I2C:16')
        send(ser, 'CNT:{}'.format(lcd_conf['contrast']))

    logger.info('Agent started')

    in_loop = True
    while True:
        sleep(SLEEP_PERIOD)
        published = False
        if len(txBuf) > 0:
            ser.write(txBuf.pop(0))
        if ser and ser.in_waiting > 0: 
            raw_data = recv(ser)
            data = raw_data.split(':')
            try:
                device_id = int(data[0][1:])
                type_ = data[1]
            except:
                type_ = NO_DATA
            if type_ != NO_DATA:
                sensor_data = data[2]
                l = sensor_data.split(',')
                if device_id == speed:
                    device_name = "speed"
                    data = pulse2speed(l[0]) 
                    speed_ph = data
                elif device_id == temp:
                    device_name = "temp"
                    data = [int(l) for l in l]
                    temp_ps = data[0]
                    humid_ps = data[1]
                elif device_id == accel:
                    device_name = "accel"
                    data = [float(l) for l in l]
                payload = dict(timestamp=int(time()),
                               thing_name=thing_name,
                               device_id=device_id,
                               device_name=device_name,
                               data=data)
                client.publish(topic, json.dumps(payload))
                published = True

            cnt_disp = cnt_disp + 1
            if cnt_disp >= DISP_PERIOD:
                cnt_disp = 0
                if skip > 0:
                    skip -= 1
                else:
                    send(ser, meter(speed_ph, temp_ps))
                    logger.info('Speed:{0:2.1f}km/h, Temp:{1:2d}C, Humid:{2:3d}%'.format(speed_ph, temp_ps, humid_ps))

        if ser_gps and ser_gps.in_waiting > 0: 
            raw_data = recv_gps(ser_gps)
            if nema_gprmc.match(raw_data):
                payload = dict(timestamp=int(time()),
                               thing_name=thing_name,
                               device_id=gps,
                               device_name='gps',
                               data=raw_data.split(','))
                client.publish(topic, json.dumps(payload))
                published = True
        
        if ser_scanner:
            barcode = recv_scanner(ser_scanner)
            if read_container_id:
                CONTAINER_ID = barcode
                read_container_id = False
                logger.info('container id read: {}'.format(CONTAINER_ID))
            elif barcode == READ_CONTAINER_ID:
                read_container_id = True
                logger.info('read container id next')
            else:
                logger.info('container ID: {}'.format(barcode))
                payload = dict(timestamp=int(time()),
                               thing_name=thing_name,
                               device_id=0,
                               device_name='scanner',
                               data=[CONTAINER_ID, barcode])
                client.publish(topic, json.dumps(payload))
                published = True

        if published:
            if cnt_adt > MAX:
                client.publish(TOPIC_AGENT, 'agent running')
                cnt_adt = 0
            else:
                cnt_adt += 1
            published = False

