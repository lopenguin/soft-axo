import tkinter as tk
from tkinter import *
from PIL import Image, ImageTk
import com, threading, time, os
from datetime import datetime
from ctypes import windll
windll.shcore.SetProcessDpiAwareness(1)

MAIN = 'main frame'
now = datetime.now()
os.mkdir('data\capture_' + now.strftime("%m-%d-%Y_%H-%M-%S"))
CAPTURE_L = 'data\capture_' + now.strftime("%m-%d-%Y_%H-%M-%S") + '\capture0.txt'
CAPTURE_R = 'data\capture_' + now.strftime("%m-%d-%Y_%H-%M-%S") + '\capture1.txt'
TERMINAL_X = 60
TERMINAL_Y = 30

# build root variable and title bar
root = Tk()
ico = Image.open('penguin.png')
photo = ImageTk.PhotoImage(ico)
root.wm_iconphoto(False, photo)
root.geometry("2000x1200")
root.title('axOS Visualizer')
root.config()
frames = {}

def build_frame():
    global frames
    # build frames -------------------------
    frames[MAIN] = Frame(root)
    frames[MAIN].pack()

    # top bar of screen
    frames[TOP] = {}
    frames[TOP][MAIN] = Frame(frames[MAIN])
    frames[TOP][MAIN].pack(side=TOP)

    frames[TOP][LEFT] = {}
    frames[TOP][LEFT][MAIN] = Frame(frames[TOP][MAIN],width= 1000, height= 100)
    frames[TOP][LEFT][MAIN].pack(side=LEFT)

    frames[TOP][RIGHT] = {}
    frames[TOP][RIGHT][MAIN] = Frame(frames[TOP][MAIN],width= 1000, height= 100)
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

build_frame()
# end build frames -------------------------

# begin command functions ------------------
def end():
    global ser1, ser2,root, stop, thread_com1
    stop = True
    if ser1 == None or ser2 == None:
        ser1 = 1
        ser2 = 1
    else:
        com.close_port(ser1)
        com.close_port(ser2)
    root.destroy
    time.sleep(1)
    if thread_com1.is_alive() or thread_com2.is_alive():
        print('WARNING: THREADS ARE STILL ACTIVE.')

        print('Attempting to quit threads by waiting 5 seconds. If terminal does not close, trash parent process.')
        time.sleep(5)

    return

def check_com1():
    global stop, ser1, data_l, capture_l
    while ser1 == None:
        time.sleep(0.01)
    time.sleep(1)
    while not stop:
        buff = com.read(ser1)
        data_l = data_l + buff
        capture_l.write(buff)
        if data_l.count('\n') > TERMINAL_Y or len(data_l) > TERMINAL_Y * TERMINAL_X:
            data_l = ''
        # time.sleep(0.001)

def check_com2():
    global stop, ser2, data_r, capture_r
    while ser2 == None:
        time.sleep(0.01)
    time.sleep(1)
    while not stop:
        buff = com.read(ser2)
        data_r = data_r  + buff
        capture_r.write(buff)
        if data_r.count('\n') > TERMINAL_Y or len(data_r) > (TERMINAL_X * TERMINAL_Y):
            data_r = ''
        # time.sleep(0.001)

def update_terminals():
    global read_l, read_r,data_l, data_r
    read_l.config(text=data_l)
    read_r.config(text=data_r)
    read_l.after(10, update_terminals)

def check_dropdowns():
    global com1, com2, ser1, ser2, com_l, send_all, send_l, send_r
    if com1.get() != 'Select port' and com2.get() != 'Select port' and com1.get() != '' and com2.get() != '' and com1.get() != com2.get():
        for entity in (send_l, send_r, send_all):
            entity['state'] = NORMAL

        if not com.is_open(ser1): ser1 = com.open_port(com1.get().split(' ')[0])
        if not com.is_open(ser2): ser2 = com.open_port(com2.get().split(' ')[0])
    
    com_l.after(100, check_dropdowns)

def write_to_serial(ser, text):
    com.write(ser, text.get(1.0, "end-1c"))
    return True

def clear():
    global read_l, read_r, write_all, write_l, write_r
    read_l.config(text='')
    read_r.config(text='')
    write_all.delete('1.0', END)
    write_l.delete('1.0', END)
    write_r.delete('1.0', END)

def check_ports():
    global com_l, com_r, com1, com2, options
    newoptions = com.serial_ports()
    # print(options)
    if [] != newoptions:
        options = newoptions
        com_r['state'] = NORMAL
        com_l['state'] = NORMAL
        
        refresh_menu(com_l, options, com1)
        refresh_menu(com_r, options, com2)
    else:
        for entity in (send_l, send_r, send_all, com_l, com_r):
            entity['state'] = DISABLED
        com1.set('Select port')
        com2.set('Select port')

    com_r.after(1000, check_ports)

def refresh_menu(menu, options, stringvar):
    menu['menu'].delete(0, 'end')

    for choice in options:
        menu['menu'].add_command(label=choice, command=tk._setit(stringvar, choice))
    

# define widgets ---------------------------
# Dropdown menu options
options = com.serial_ports()
if options == []: options = ['']
  
# datatype of menu text
com1 = StringVar()
com2 = StringVar()
  
# initial menu text
com1.set("Select port")
com2.set("Select port")
ser1 = None
ser2 = None
  
# Create Dropdown menus
com_l = OptionMenu(frames[TOP][LEFT][MAIN] , com1 , *options)
com_r = OptionMenu(frames[TOP][RIGHT][MAIN] , com2 , *options)
data_l = ''
data_r = ''

# Create serial read terminals
read_l = Label(frames[BOTTOM][TOP][LEFT][TOP][MAIN], height = TERMINAL_Y, width = TERMINAL_X, anchor='nw', bg='black',fg='white',justify=LEFT,wraplength=1050, text = data_l, font=('Lucida Console',8))
read_r = Label(frames[BOTTOM][TOP][RIGHT][TOP][MAIN], height = TERMINAL_Y, width = TERMINAL_X, anchor='nw',bg='black',fg='white',justify=LEFT,wraplength=1050, text = data_r, font=('Lucida Console',8))

# Create individual serial write terminals
write_l = Text(frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT][MAIN], height = 1, width = 40,borderwidth=4)
write_r = Text(frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT][MAIN], height = 1, width = 40,borderwidth=4)

# create individual serial write buttons
send_l = Button(frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT][MAIN], text = "Send", command = lambda : write_to_serial(ser1, write_l), borderwidth=4)
send_r = Button(frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT][MAIN], text = "Send", command = lambda : write_to_serial(ser2, write_r),borderwidth=4)
 
# Create master send/write.
clear_all = Button(frames[BOTTOM][BOTTOM][LEFT][LEFT][MAIN], text = "Clear all",command = clear,borderwidth=4)
write_all = Text(frames[BOTTOM][BOTTOM][LEFT][RIGHT][MAIN], height = 1, width = 40,borderwidth=4)
send_all = Button(frames[BOTTOM][BOTTOM][RIGHT][MAIN], text = "Send to both", command=lambda : write_to_serial(ser1, write_all) and write_to_serial(ser2, write_all),borderwidth=4)


# pack tk entities
com_r.pack()
com_r.place(relx=0.5,rely=0.5,anchor=CENTER)
com_l.pack()
com_l.place(relx=0.5,rely=0.5,anchor=CENTER)
read_l.pack()
read_r.pack()
write_l.pack()
write_r.pack()
send_l.pack()
send_r.pack()
clear_all.pack()
write_all.pack()
send_all.pack()

# disable serial interaction until we connect with a port
for entity in (send_l, send_r, send_all, com_r, com_l):
    entity['state'] = DISABLED

# set up polling
check_dropdowns()
update_terminals()
check_ports()

# multithread the serial polling for speed
thread_com1 = threading.Thread(target=check_com1)
thread_com2 = threading.Thread(target=check_com2)
thread_com1.start()
thread_com2.start()
stop = False

# data wrting files
capture_l = open(CAPTURE_L, 'w', encoding='utf-8')
capture_r = open(CAPTURE_R, 'w', encoding='utf-8')
 
tk.mainloop()

# wrapping things up
end()
capture_l.close()
capture_r.close()