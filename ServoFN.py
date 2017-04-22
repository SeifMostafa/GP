import RPi.GPIO as GPIO
import time

#GPIO.cleanup()

GPIO.setmode(GPIO.BOARD)

GPIO.setup(12, GPIO.OUT)

p = GPIO.PWM(12, 50)

p.start(7.5)

try:
		p.ChangeDutyCycle(7.5)  # turn towards 90 degree
		time.sleep(1) # sleep 1 second
		p.stop()
		GPIO.cleanup()


except KeyboardInterrupt:
        p.stop()
        GPIO.cleanup()
