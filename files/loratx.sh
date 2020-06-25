#!/bin/sh

. /etc/init.d/loratx

init_reset_gpio


loratx /etc/conf.json

exit 0