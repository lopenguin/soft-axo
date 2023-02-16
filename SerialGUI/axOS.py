import window
import tkinter as tk
from visualize import *
from com import Com
from penguin import Penguins


COM1 = 'COM18'
COM2 = 'COM19'
GRAPH_ROWS = 1
GRAPH_COLUMNS = 1
SCREEN_SIZE = str(GRAPH_COLUMNS * 380 + 300) + 'x' + str(GRAPH_ROWS * 150)
NUM_GRAPHS = GRAPH_ROWS * GRAPH_COLUMNS
PORTS = ['/dev/cu.usbmodem6A_35_4A_B71']


types =     [Com,
            ]

args =      [[COM1],
            ]

assert(len(types) == len(args))


# Penguins.amberlab()
print("\nWelcome to axOS :)")
root = tk.Tk()
root.geometry(SCREEN_SIZE)
app = window.Window(vis_types=types,
                    vis_args=args,
                    master=root,
                    graph_rows=GRAPH_COLUMNS,
                    graph_columns=GRAPH_ROWS)
tk.mainloop()

