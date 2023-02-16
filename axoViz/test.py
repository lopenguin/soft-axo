import tkinter as tk
from tkinter import *
MAIN = 'main frame'

# build root variables
root = Tk()
root.geometry("1000x500")

# build frames
frames = {}
frames[MAIN] = Frame(root)
frames[MAIN].pack()
frames[TOP] = Frame(frames[MAIN])
frames[TOP].pack(side=TOP)
frames[BOTTOM] = {}
frames[BOTTOM][MAIN] = Frame(frames[MAIN])
frames[BOTTOM][MAIN].pack(side=BOTTOM)

frames[BOTTOM][LEFT] = {}
frames[BOTTOM][LEFT][MAIN] = Frame(frames[BOTTOM][MAIN])
frames[BOTTOM][LEFT][MAIN].pack(side=LEFT)
frames[BOTTOM][LEFT][BOTTOM] = Frame(frames[BOTTOM][LEFT][MAIN])
frames[BOTTOM][LEFT][BOTTOM].pack(side=BOTTOM)

frames[BOTTOM][RIGHT] = {}
frames[BOTTOM][RIGHT][MAIN] = Frame(frames[BOTTOM][MAIN])
frames[BOTTOM][RIGHT][MAIN].pack(side=RIGHT)
frames[BOTTOM][RIGHT][BOTTOM] = Frame(frames[BOTTOM][RIGHT][MAIN])
frames[BOTTOM][RIGHT][BOTTOM].pack(side=BOTTOM)

 
# Create text widget and specify size.
terminal_1 = Text(frames[BOTTOM][LEFT][MAIN], height = 5, width = 50)
terminal_2 = Text(frames[BOTTOM][RIGHT][MAIN], height = 5, width = 50)
 
# Create label
l = Label(frames[TOP], text = 'axOS')
l.config(font =("Microsoft JhengHei Light", 36))
 
Fact = """A man can be arrested in
Italy for wearing a skirt in public."""
 
# Create button for next text.
b1 = Button(frames[BOTTOM][LEFT][BOTTOM], text = "Next", )
 
# Create an Exit button.
b2 = Button(frames[BOTTOM][RIGHT][BOTTOM], text = "Exit",
            command = root.destroy)
 
l.pack()
terminal_1.pack(side = LEFT)
terminal_2.pack(side = RIGHT)
b1.pack(side = BOTTOM)
b2.pack(side = BOTTOM)
 
# Insert The Fact.
# T.insert(tk.END, Fact)
 
tk.mainloop()