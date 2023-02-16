
# Python Program to make a scrollable frame
# using Tkinter
  
from tkinter import *
  
class ScrollBar:
     
    # constructor
    def __init__(self):
         
        # create root window
        root = Tk()
  
        # create a horizontal scrollbar by
        # setting orient to horizontal
        h = Scrollbar(root, orient = 'horizontal')
  
        # attach Scrollbar to root window at
        # the bootom
        h.pack(side = BOTTOM, fill = X)
  
        # create a vertical scrollbar-no need
        # to write orient as it is by
        # default vertical
        v = Scrollbar(root)
  
        # attach Scrollbar to root window on
        # the side
        v.pack(side = RIGHT, fill = Y)
          
  
        # create a Text widget with 15 chars
        # width and 15 lines height
        # here xscrollcomannd is used to attach Text
        # widget to the horizontal scrollbar
        # here yscrollcomannd is used to attach Text
        # widget to the vertical scrollbar
        t = Text(root, width = 15, height = 15, wrap = NONE,
                 xscrollcommand = h.set,
                 yscrollcommand = v.set)
  
        # insert some text into the text widget
        for i in range(20):
            t.insert(END,"this is some text\n")
  
        # attach Text widget to root window at top
        t.pack(side=TOP, fill=X)
  
        # here command represents the method to
        # be executed xview is executed on
        # object 't' Here t may represent any
        # widget
        h.config(command=t.xview)
  
        # here command represents the method to
        # be executed yview is executed on
        # object 't' Here t may represent any
        # widget
        v.config(command=t.yview)
  
        # the root window handles the mouse
        # click event
        root.mainloop()
 
# create an object to Scrollbar class
s = ScrollBar()
        