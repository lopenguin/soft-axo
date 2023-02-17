import time, serial
PORT1 = 'COM19'
PORT2 = None


ser1 = serial.Serial(PORT1, 59200, timeout=1)
ser2 = serial.Serial(PORT2, 59200, timeout=1)


# output = input('Enable output (ser1/ser2/n)?') # figure out output 
# ser = ser1 if output == 'ser1' else (ser2 if output == 'ser2' else None)
# if ser != None:


while True:
    # if output == 'y':
    line = ser1.readline()  
    if line.decode() != '':
        print(line, line.decode())
    # towrite = input('Command: ')
    # ser1.write(bytes(towrite, 'ascii'))
    # ser2.write(bytearray(towrite))
    time.sleep(0.01)