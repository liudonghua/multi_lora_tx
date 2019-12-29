#!/bin/sh

echo "0" > /sys/class/gpio/gpio3/value

usleep 5000

echo "1" > /sys/class/gpio/gpio3/value

sleep 5

loratx /etc/conf.json

exit 0