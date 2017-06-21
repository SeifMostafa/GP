import time
import sys
import serial

arrived = False
connected = False
ser=serial.Serial("/dev/ttyUSB0",9600)

while not connected:
    serin = ser.read()
    connected =True

desiredPosition=""+ sys.argv[1]
ser.write(desiredPosition)
time.sleep(5)

while not arrived:
    serin = ser.read()
    if serin == "$":
        file = open("/home/pi/Documents/REPO/GP/reached","w")
        file.write("1")
        file.close()
        arrived =True
ser.close

