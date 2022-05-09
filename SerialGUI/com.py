#!/usr/bin/python

import os, io, serial
import numpy as np
from bcolors import *
from visualize import Visualization
from matplotlib import pyplot as plt
from matplotlib import animation

class Com(Visualization):

    BAUD = 9800
    TIMEOUT = 1
    SIZE = 100
    CONNECTED = False
    MAX_SIZE = 100

    def __init__(self, fig, nrows, ncols, index, args=[]):
        self.fig = fig
        self.x = [0]
        self.ax = self.fig.add_subplot(nrows, ncols, index)
        self.line, = self.ax.plot([], [])

        print(bcolors.BOLD + "INITIALIZING PORT..." + bcolors.ENDC)
        assert(len(args) > 0)
        self.port = args[0]
        self.ser = None
        self.sio = None
        self.data = [0]
        self.open_port()


    def init_animation(self):
        self.ani = animation.FuncAnimation(self.fig, self.animate, np.arange(1, 100), interval=25, blit=False)
        return self.ani


    def animate(self, _):
        self.read_port()
        if (len(self.x) < self.MAX_SIZE):
            self.line.set_xdata(self.x)
            self.line.set_ydata(self.data)
            self.ax.set_xlim(0, 1+self.x[-1])
            self.ax.set_ylim(-1.5, 1.5) 
        else:
            self.line.set_xdata(self.x[-self.MAX_SIZE:-1])
            self.line.set_ydata(self.data[-self.MAX_SIZE:-1])
            self.ax.set_xlim(self.x[-self.MAX_SIZE], self.x[-1])
            self.ax.set_ylim(-1.5, 1.5) 
        
        return self.line,


    def open_port(self):
        try:
            self.ser = serial.Serial()
            self.ser.baudrate = 9800
            self.ser.port = self.port
            self.ser.timeout = self.TIMEOUT
            self.ser.open()
            print(bcolors.OKGREEN + "Opening " + self.ser.name + " with baudrate " + str(self.BAUD) + bcolors.ENDC)
            self.sio = io.TextIOWrapper(io.BufferedRWPair(self.ser, self.ser))
        except:
            print(bcolors.FAIL + "Invalid port found at " + self.port + ". Please check if port is connected." + bcolors.ENDC)
            print("Listing available serial ports here:")
            print("++++")
            os.system('python3 -m serial.tools.list_ports')
            print("++++")
            print("Ending process...")
            quit()
                    

    def read_port(self):
        self.sio.flush()
        b = (self.ser.readline()).decode("utf-8")
        if (b == '' or b[0:-2] == ''):
            return
        val = float(b[0:-2])
        self.x.append(self.x[-1] + 1)
        self.data.append(val)