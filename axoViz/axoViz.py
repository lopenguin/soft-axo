import tkinter as tk
from tkinter import *
from PIL import Image, ImageTk
import com, threading, time, sys

MAIN = 'main frame'

# build root variable and title bar
root = Tk()
ico = Image.open('penguin.png')
photo = ImageTk.PhotoImage(ico)
root.wm_iconphoto(False, photo)
root.geometry("1000x600")
root.title('axOS Visualizer')

# build frames -------------------------
frames = {}
frames[MAIN] = Frame(root)
frames[MAIN].pack()

# top bar of screen
frames[TOP] = {}
frames[TOP][MAIN] = Frame(frames[MAIN])
frames[TOP][MAIN].pack(side=TOP)

frames[TOP][LEFT] = {}
frames[TOP][LEFT][MAIN] = Frame(frames[TOP][MAIN],padx=180)
frames[TOP][LEFT][MAIN].pack(side=LEFT)

frames[TOP][RIGHT] = {}
frames[TOP][RIGHT][MAIN] = Frame(frames[TOP][MAIN],padx=180)
frames[TOP][RIGHT][MAIN].pack(side=RIGHT)


# bottom (rest of) screen
frames[BOTTOM] = {}
frames[BOTTOM][MAIN] = Frame(frames[MAIN])
frames[BOTTOM][MAIN].pack(side=BOTTOM)

# dual terminal
frames[BOTTOM][TOP] = {}
frames[BOTTOM][TOP][MAIN] = Frame(frames[BOTTOM][MAIN])
frames[BOTTOM][TOP][MAIN].pack(side = TOP)

# left terminal interface
frames[BOTTOM][TOP][LEFT] = {}
frames[BOTTOM][TOP][LEFT][MAIN] = Frame(frames[BOTTOM][TOP][MAIN])
frames[BOTTOM][TOP][LEFT][MAIN].pack(side = LEFT)

frames[BOTTOM][TOP][LEFT][TOP] = {}
frames[BOTTOM][TOP][LEFT][TOP][MAIN] = Frame(frames[BOTTOM][TOP][LEFT][MAIN], padx=20, pady=10)
frames[BOTTOM][TOP][LEFT][TOP][MAIN].pack(side = TOP)

frames[BOTTOM][TOP][LEFT][BOTTOM] = {}
frames[BOTTOM][TOP][LEFT][BOTTOM][MAIN] = Frame(frames[BOTTOM][TOP][LEFT][MAIN])
frames[BOTTOM][TOP][LEFT][BOTTOM][MAIN].pack(side = BOTTOM)

frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT] = {}
frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT][MAIN] = Frame(frames[BOTTOM][TOP][LEFT][BOTTOM][MAIN], padx=20)
frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT][MAIN].pack(side = LEFT)

frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT] = {}
frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT][MAIN] = Frame(frames[BOTTOM][TOP][LEFT][BOTTOM][MAIN])
frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT][MAIN].pack(side = RIGHT)

# right terminal interface
frames[BOTTOM][TOP][RIGHT] = {}
frames[BOTTOM][TOP][RIGHT][MAIN] = Frame(frames[BOTTOM][TOP][MAIN])
frames[BOTTOM][TOP][RIGHT][MAIN].pack(side = RIGHT)

frames[BOTTOM][TOP][RIGHT][TOP] = {}
frames[BOTTOM][TOP][RIGHT][TOP][MAIN] = Frame(frames[BOTTOM][TOP][RIGHT][MAIN], padx=20, pady=10)
frames[BOTTOM][TOP][RIGHT][TOP][MAIN].pack(side = TOP)

frames[BOTTOM][TOP][RIGHT][BOTTOM] = {}
frames[BOTTOM][TOP][RIGHT][BOTTOM][MAIN] = Frame(frames[BOTTOM][TOP][RIGHT][MAIN])
frames[BOTTOM][TOP][RIGHT][BOTTOM][MAIN].pack(side = BOTTOM)

frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT] = {}
frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT][MAIN] = Frame(frames[BOTTOM][TOP][RIGHT][BOTTOM][MAIN], padx=20)
frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT][MAIN].pack(side = LEFT)

frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT] = {}
frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT][MAIN] = Frame(frames[BOTTOM][TOP][RIGHT][BOTTOM][MAIN])
frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT][MAIN].pack(side = RIGHT)

# bottom bar of screen
frames[BOTTOM][BOTTOM] = {}
frames[BOTTOM][BOTTOM][MAIN] = Frame(frames[BOTTOM][MAIN], padx=20, pady=40)
frames[BOTTOM][BOTTOM][MAIN].pack(side=BOTTOM)

# left side of bottom bar
frames[BOTTOM][BOTTOM][LEFT] = {}
frames[BOTTOM][BOTTOM][LEFT][MAIN] = Frame(frames[BOTTOM][BOTTOM][MAIN])
frames[BOTTOM][BOTTOM][LEFT][MAIN].pack(side=LEFT)

frames[BOTTOM][BOTTOM][LEFT][LEFT] = {}
frames[BOTTOM][BOTTOM][LEFT][LEFT][MAIN] = Frame(frames[BOTTOM][BOTTOM][LEFT][MAIN],padx=10)
frames[BOTTOM][BOTTOM][LEFT][LEFT][MAIN].pack(side=LEFT)

frames[BOTTOM][BOTTOM][LEFT][RIGHT] = {}
frames[BOTTOM][BOTTOM][LEFT][RIGHT][MAIN] = Frame(frames[BOTTOM][BOTTOM][LEFT][MAIN],padx=10)
frames[BOTTOM][BOTTOM][LEFT][RIGHT][MAIN].pack(side=RIGHT)

# right side of bottom bar
frames[BOTTOM][BOTTOM][RIGHT] = {}
frames[BOTTOM][BOTTOM][RIGHT][MAIN] = Frame(frames[BOTTOM][BOTTOM][MAIN],padx=10)
frames[BOTTOM][BOTTOM][RIGHT][MAIN].pack(side=RIGHT)

# end build frames -------------------------

# begin command functions ------------------
def end():
    global ser1, ser2,root, stop, thread_com1
    stop = True
    if ser1 == None or ser2 == None:
        ser1 = 1
        ser2 = 1
    time.sleep(2)
    com.close_port(ser1)
    com.close_port(ser2)
    root.destroy
    if thread_com1.is_alive() or thread_com2.is_alive():
        print('WARNING: THREADS ARE STILL ACTIVE. TRASH TERMINAL!!!')
    input('Press enter to quit...')
    return

def check_com1():
    global stop, ser1, data_l
    while ser1 == None:
        time.sleep(0.01)
    time.sleep(1)
    while not stop:
        data_l.set(data_l.get() + com.read(ser1))
        if len(data_l.get()) > 900:
            data_l.set('')
        time.sleep(0.01)


def check_com2():
    global stop, ser2, data_r
    while ser2 == None:
        time.sleep(0.01)
    time.sleep(1)
    while not stop:
        data_r.set(data_r.get() + com.read(ser2))
        if len(data_r.get()) > 900:
            data_r.set('')
        time.sleep(0.01)

def check_dropdowns():
    global com1, com2, ser1, ser2, com_l
    if com1.get() != 'Select Wixel 1' and com2.get() != 'Select Wixel 2':
        ser1 = com.open_port(com1.get())
        ser2 = com.open_port(com2.get())
    else:
        com_l.after(100, check_dropdowns)

def write_to_serial(ser, text):
    com.write(ser, text.get(1.0, "end-1c"))

# define widgets ---------------------------
# Dropdown menu options
options = com.serial_ports()
  
# datatype of menu text
com1 = StringVar()
com2 = StringVar()
  
# initial menu text
com1.set("Select Wixel 1")
com2.set("Select Wixel 2")
opened = False
ser1 = None
ser2 = None
  
# Create Dropdown menu
com_l = OptionMenu( frames[TOP][LEFT][MAIN] , com1 , *options)
com_l.pack()
data_l = StringVar()
data_r = StringVar()
# data_l.trace('w', write_to_com1)

com_r = OptionMenu( frames[TOP][RIGHT][MAIN] , com2 , *options)
com_r.pack()

# Create serial read terminals
read_l = Label(frames[BOTTOM][TOP][LEFT][TOP][MAIN], height = 20, width = 50, anchor='nw', bg='black',fg='white',justify=LEFT,wraplength=350, textvariable = data_l)
read_r = Label(frames[BOTTOM][TOP][RIGHT][TOP][MAIN], height = 20, width = 50, anchor='nw',bg='black',fg='white',justify=LEFT,wraplength=350, textvariable = data_r)

# Create individual serial write terminals
write_l = Text(frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT][MAIN], height = 1, width = 40)
write_r = Text(frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT][MAIN], height = 1, width = 40)

# create individual serial write buttons
send_l = Button(frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT][MAIN], text = "Send", command = lambda : write_to_serial(ser1, write_l))
send_r = Button(frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT][MAIN], text = "Send", command = lambda : write_to_serial(ser2, write_r))
 
# Create master send/write.
clear_all = Button(frames[BOTTOM][BOTTOM][LEFT][LEFT][MAIN], text = "Clear all",command = end)
write_all = Text(frames[BOTTOM][BOTTOM][LEFT][RIGHT][MAIN], height = 1, width = 40)
send_all = Button(frames[BOTTOM][BOTTOM][RIGHT][MAIN], text = "Send to both", command = end)



com_r.pack()
com_l.pack()
read_l.pack(expand=True,fill='both')
read_r.pack(expand=True,fill='both')
write_l.pack()
write_r.pack()
send_l.pack()
send_r.pack()
clear_all.pack()
write_all.pack()
send_all.pack()
 

check_dropdowns()

thread_com1 = threading.Thread(target=check_com1)
thread_com2 = threading.Thread(target=check_com2)
thread_com1.start()
thread_com2.start()
stop = False
 
tk.mainloop()
end()