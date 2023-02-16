import tkinter as tk
from tkinter import *
from PIL import Image, ImageTk

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

# define widgets ---------------------------

# Create axOS title
title = Label(frames[TOP][MAIN], text = 'axOS')
title.config(font =("Microsoft JhengHei Light", 36))

# Create serial read terminals
read_l = Text(frames[BOTTOM][TOP][LEFT][TOP][MAIN], height = 20, width = 50)
read_r = Text(frames[BOTTOM][TOP][RIGHT][TOP][MAIN], height = 20, width = 50)

# Create individual serial write terminals
write_l = Text(frames[BOTTOM][TOP][LEFT][BOTTOM][LEFT][MAIN], height = 1, width = 40)
write_r = Text(frames[BOTTOM][TOP][RIGHT][BOTTOM][LEFT][MAIN], height = 1, width = 40)

# create individual serial write buttons
send_l = Button(frames[BOTTOM][TOP][LEFT][BOTTOM][RIGHT][MAIN], text = "Send L", )
send_r = Button(frames[BOTTOM][TOP][RIGHT][BOTTOM][RIGHT][MAIN], text = "Send R", )
 
# Create master send/write.
clear_all = Button(frames[BOTTOM][BOTTOM][LEFT][LEFT][MAIN], text = "Clear",command = root.destroy)
write_all = Text(frames[BOTTOM][BOTTOM][LEFT][RIGHT][MAIN], height = 1, width = 40)
send_all = Button(frames[BOTTOM][BOTTOM][RIGHT][MAIN], text = "Send", command = root.destroy)


 
title.pack(side = TOP)
read_l.pack()
read_r.pack()
write_l.pack()
write_r.pack()
send_l.pack()
send_r.pack()
clear_all.pack()
write_all.pack()
send_all.pack()
 
# Insert The Fact.
# T.insert(tk.END, Fact)
 
tk.mainloop()