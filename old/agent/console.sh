#!/usr/bin/env bash

PORT=usb-FTDI_FT230X_Basic_UART_DJ00LTN7-if00-port0
BAUDRATE=9600
python -m serial.tools.miniterm /dev/serial/by-id/$PORT $BAUDRATE 
