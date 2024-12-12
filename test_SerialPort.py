""" test reading serial port in Linux
"""

"""
import serial
import time


def readserial(comport, baudrate, timestamp=False):

    ser = serial.Serial(comport, baudrate, timeout=0.1)         # 1/timeout is the frequency at which the port is read

    while True:

        data = ser.readline().decode().strip()

        if data and timestamp:
            timestamp = time.strftime('%H:%M:%S')
            print(f'{timestamp} > {data}')
        elif data:
            print(data)


if __name__ == '__main__':

    readserial('/dev/ttyUSB0', 115200, True)

    print("Read serial port complete.")

    print("Read Serial port completed.")
"""

import io

import pynmea2
import serial


ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.0001)
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))

"""
while 1:
    try:
        line = sio.readline()
        print(line)
        msg = pynmea2.parse(line)
        print(repr(msg))
    except serial.SerialException as e:
        print('Device error: {}'.format(e))
        break
    except pynmea2.ParseError as e:
        print('Parse error: {}'.format(e))
        continue

"""
while 1:
    line = sio.readline()
    if (line == ''):
        pass
    elif(line):
        print(line)
    #msg = pynmea2.parse(line)
    #print(repr(msg))

"""
import serial,time,pynmea2

port = '/dev/ttyUSB0'
baud = 115200

serialPort = serial.Serial(port, baudrate = baud, timeout = 0.1)
while True:
    
    str = ''
    try:
        str = serialPort.readline().decode().strip()
    except Exception as e:
        print(e)
    if(str != ''):
        print(str)
    
    if str.find('GGA') > 0:
        try:
            msg = pynmea2.parse(str)
            print(msg.timestamp,'Lat:',round(msg.latitude,6),'Lon:',round(msg.longitude,6),'Alt:',msg.altitude,'Sats:',msg.num_sats)
        except Exception as e:
            print(e)
"""
