import serial, sys, glob
import serial.tools.list_ports

def serial_ports():
    ret = []
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        ret.append(p)

    return ret


def open_port(port, BAUD=57600, timeout=0.01):
    ser = serial.Serial(port, baudrate=BAUD, timeout=timeout)  # open serial port
    print(ser.name)         # check which port was really used
    return ser


def close_port(ser):
    if ser.isOpen() == True:
        ser.close()
    else:
        print(f'Warning: port {ser.name} is already closed')


def is_open(ser):
    return ser != None and ser.isOpen()


def read(ser):
    # line = ser.readline()
    # buff = str(line, 'ISO-8859-1') # would be technically correct to switch to utf-8 or utf-16
    # if buff != '':
    #     return buff[:-1]
    # else:
    #     return ''
    bchar = ser.read()
    char = chr(int.from_bytes(bchar, 'big'))
    return str(char if (char != '\r' and char != '\x00') else '')
    # return 'a'


def write(ser, text):
    if len(text) == 0: return
    ser.write(bytes(text, 'ascii'))