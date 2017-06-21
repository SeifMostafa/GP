import RPi.GPIO as GPIO
import time
import os
GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.IN,pull_up_down=GPIO.PUD_UP)
while True:
	state = GPIO.input(18)
    if state == False:
        os.system("sudo shutdown -h now")
    time.sleep(0.10)
