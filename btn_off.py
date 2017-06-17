import RPi.GPIO as GPIO
import time
import os
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN,pull_up_down=GPIO.PUD_UP)
while True:
    if GPIO.input(17):
        os.system("sudo shutdown -h now")
    prev_input = input
    time.sleep(0.10)
