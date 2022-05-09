import window
import tkinter as tk
from visualize import *
from com import Com
from penguin import Penguins


COM1 = '/dev/cu.usbmodem144401'
COM2 = None
SCREEN_SIZE = "1200x400"
GRAPH_ROWS = 1
GRAPH_COLUMNS = 1
NUM_GRAPHS = GRAPH_ROWS * GRAPH_COLUMNS
PORTS = ['/dev/cu.usbmodem141201']


types =     [SimpleVisualization,
            ]

args =      [[COM1],
            ]

assert(len(types) == len(args))


Penguins.amberlab()
print("\nWelcome to axOS :)")
root = tk.Tk()
root.geometry(SCREEN_SIZE)
app = window.Window(vis_types=types,
                    vis_args=args,
                    master=root,
                    graph_rows=GRAPH_ROWS,
                    graph_columns=GRAPH_COLUMNS)
tk.mainloop()

