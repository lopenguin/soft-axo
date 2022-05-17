FILENAME = 'capture.txt'

MESSAGE_FILENAME = 'capture_message.txt'
DATA_FILENAME = 'data.csv'

import os, csv

from bcolors import *
from util import *

# edit your key to be whatever you like!
keys = {'\nF' : [(4, uint_to_uint, ()), (2, uint_to_uint, ())],

        '\nL' : [(4, uint_to_uint, ()), (2, uint_to_uint, ())],

        '\nM' : [(4, uint_to_uint, ()),
                 (2, uint_to_uint, ()),
                 (2, uint_to_uint, ()),
                 (2, uint_to_int, (16)),
                 (2, uint_to_uint, ()),
                 (2, uint_to_uint, ()),
                 (2, uint_to_uint, ()),
                 (2, uint_to_uint, ()),
                 (2, uint_to_uint, ())],

        '\nI' : [(1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (1, uint_to_uint, ()),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 4)),
                 (2, uint_to_float, (16, 2)),
                 (2, uint_to_float, (16, 2)),
                 (2, uint_to_float, (16, 2)),
                 (2, uint_to_float, (16, 2)),
                 (2, uint_to_float, (16, 2)),
                 (2, uint_to_float, (16, 2))]}

class Reader:

    """
    key : dictionary mapping header to encoding type
    read : function that accepts a counter and returns a byte of data
    msg_filename : file to write the message to
    data_filename : file to write the data to
    """
    def __init__(self, keys, read, msg_filename, data_filename, args):

        # file stuff...
        self.msg_filename = msg_filename
        self.data_filename = data_filename
        self.data_file = open(DATA_FILENAME, 'w')
        self.data_csv = csv.writer(self.data_file, delimiter=',',lineterminator='\n')
        
        # dynamic axodata members
        self.args = args
        self.message = ''
        self.keys = keys
        self.data = {}
        self.buffer = ''
        if not read:
            self.readByte = self.readByteDefault
        self.readByte = read
        for elem in self.keys:
            self.data[elem] = []

        # random stuff...
        self.bytesread = 0
        self.shutdown = False
        self.parity = True
        self.loading = 0


    def callback(self, key):
        # print("in")
        self.safeRead()
        row = []
        byteslist = self.keys[key]
        for (bytes, f, args) in byteslist:
            for _ in range(bytes):
                self.safeRead()
            value = f(int('0x' + self.buffer[-2 * bytes:], 16), args)
            row.append(value)
        self.data[key].append(row)
        self.data_csv.writerow(row)
        self.buffer = ''

    def hexToASCII(self, str):
        assert(len(str) == 2)
        return chr(int('0x' + str, 16))
    
    """
    Function should read a single byte from whatever source you have. It should
    return it as a character or string
    """
    def readByteDefault(self) -> str:
        raise NotImplementedError

    def safeRead(self):
        h = self.readByte(self.args)
        if h == -1:
            self.shutdown = True
            return
        
        self.buffer += h
        self.parity = not self.parity
        if self.parity and len(self.buffer) > 2:
            self.message += self.hexToASCII(self.buffer[-2:])

    def halt(self):
        msg_file = open(self.msg_filename, 'w')
        
        msg_file.write(self.message)

        print(bcolors.FAIL+'Recieved shutdown signal. Terminating...'+bcolors.ENDC)
        self.data_file.close()
        msg_file.close()

    def spin(self):
        while True:
            if self.shutdown:
                self.halt()
                quit()

            self.safeRead()
            self.bytesread += 0.5
            if len(self.buffer) < 4:
                continue

            key = self.hexToASCII(self.buffer[-2:]) # convert last two bytes to ASCII
            header = self.hexToASCII(self.buffer[-4:-2]) + key # convert last four bytes to ASCII string

            if header in self.keys:
                self.callback(header)

            if int(self.bytesread) == self.bytesread and int(self.bytesread) % 10000 == 0:
                os.system('cls' if os.name == 'nt' else 'clear')
                spin = ''
                if self.loading == 0:
                    spin = 'Spinning.'
                elif self.loading == 1:
                    spin = 'Spinning..'
                elif self.loading == 2:
                    spin = 'Spinning...'
                elif self.loading == 3:
                    spin = 'Spinning....'
                    self.loading = -1
                print(bcolors.OKGREEN+spin+bcolors.ENDC)
                self.loading += 1
                print(str(int(self.bytesread)) + ' bytes read.')

    def getData(self, key):
        try:
            return data[key]
        except Exception as e:
            print(e)


# main code....
with open(FILENAME) as file:
    data = file.readlines()[0]

def read(args):
    args[0] += 1
    if args[0] >= len(data):
        return -1
    return data[args[0]]

node = Reader(keys, read, MESSAGE_FILENAME, DATA_FILENAME, [-1])
node.spin()