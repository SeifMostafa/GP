#!/usr/bin/env python

import os
from time import sleep

import RPi.GPIO as GPIO

os.system('omxplayer -o local /home/pi/Documents/GP/huawei.mp3')
sleep(0.1)
