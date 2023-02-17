import serial, sys, glob

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


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
    char = int.from_bytes(bchar, 'big')
    return str(char if char != '\r' else '')
    # return 'a'


def write(ser, text):
    if len(text) == 0: return
    ser.write(bytes(text, 'ascii'))