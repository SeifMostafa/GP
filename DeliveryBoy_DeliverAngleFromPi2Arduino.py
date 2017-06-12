import time
import sys
import serial
connected = False
ser=serial.Serial("/dev/ttyUSB0",9600)
while not connected:
    serin = ser.read()
    connected =True


counter=""+ sys.argv[1]
ser.write(counter)
time.sleep(5)




ser.close
