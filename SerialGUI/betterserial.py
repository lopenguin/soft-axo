#!/usr/bin/python

import os, serial, io, time
import threaded
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

PORT = '/dev/cu.usbmodem141201'
BAUD = 9800
TIMEOUT = 1
SIZE = 100
CONNECTED = False
# this is so cool!
data = [2.0 * i * 1.0 / SIZE - 1 for i in range(SIZE)]
# this is so cool!

def updateplot(cnt, plot):
    plot.set_xdata([i for i in range(len(data))])
    plot.set_ydata(data)

@threaded.Threaded
def make_port():
    if (PORT == ''):
        print("No port listed. Available Serial Ports:")
        os.system('python3 -m serial.tools.list_ports')
    else:
        with serial.Serial() as ser:
            ser.baudrate = 9800
            ser.port = PORT
            ser.timeout = TIMEOUT
            ser.open()
            print("Opening ", ser.name, " with baudrate ", BAUD)
            CONNECTED = True
            sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
            while True:
                sio.flush()
                b = (ser.readline()).decode("utf-8")
                if (b == ''):
                    continue
                val = float(b[0:-2])
                if (len(data) < SIZE):
                    data.append(val)
                else:
                    data.pop(0)
                    data.append(val)
                print(val)

    

# @threaded.Threaded
def drawplot():
    print("running")
    while not True:
        print("Waiting for connection...") 
        time.sleep(0.5)
    fig = plt.figure()
    plot = plt.plot([i for i in range(len(data))], data)
    ani = FuncAnimation(fig, updateplot, fargs=(plot))
    plt.show()

try:
    t1 = make_port()
    # t2 = drawplot()
    print("Connecting to xport")
    t1.start()
    print("Starting graph")
    # t2.start()
    # t1.join()
#    t2.join()
except:
    print("Error: unable to start thread")

drawplot()