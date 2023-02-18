import os, pty, serial, time

master, slave = pty.openpty()
s_name = os.ttyname(slave)
print(s_name)

ser = serial.Serial(s_name)

while True:
    # To Write to the device
    print('writing')
    ser.write(b'Your text')

    # To read from the device
    os.read(master,1000)
    time.sleep(1.0)