#---------Imports

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from visualize import *
#---------End of imports



from tkinter import Frame,Label,Entry,Button


class Window(Frame):

    def __init__(self, vis_types=None, vis_args=None, master = None, graph_rows=1, graph_columns=1):       
        Frame.__init__(self, master)
        self.master = master
        self.sub = tk.Frame(master)
        self.graph_rows = graph_rows
        self.graph_columns = graph_columns
        self.num_graphs = graph_columns * graph_rows
        if (not vis_types):
            self.vis_types = []
            for i in range(graph_columns * graph_rows):
                self.vis_types.append(SimpleVisualization)
        else:
            self.vis_types = vis_types
        
        self.vis_args = []
        if (not vis_args):
            for i in range(graph_columns * graph_rows):
                self.vis_args.append([])
        else:
            self.vis_args = vis_args
        self.init_window()                              


    def clear(self):      
        print("--- OUTPUT CLEARED ---")
        self.textSerial.delete(0, len(self.textSerial.get()))
        self.terminal.delete(1.0, 'end')



    def send(self):
        print("Sent", self.textSerial.get())
        self.textSerial.delete(0, len(self.textSerial.get()))


    def init_window(self):

        self.master.title("axOS")
        self.pack(fill='both', expand=1)

        self.buttonSend = Button(self,text="Send",command=self.send,width=12)        
        self.buttonSend.grid(row=(self.graph_rows + 1),column=(self.graph_columns + 1))
        self.buttonClear = Button(self,text="Clear",command=self.clear,width=12)
        self.buttonClear.grid(row=(self.graph_rows + 3),column=(self.graph_columns + 1))

        self.textSerial = Entry(self,width=30)
        self.textSerial.grid(row=(self.graph_rows + 2),column=(self.graph_columns + 1))

        self.terminal = tk.Text(self, width=50, height=10)
        self.terminal.grid(row=0,column=(self.graph_columns + 1))

        self.terminal.insert('1.0', 'Initialized...')
        self.buttonClear.bind(lambda e:self.Clear)


        self.fig = plt.Figure()
        self.FSR_fig = plt.Figure()

        self.figures = []
        self.vis = []
        self.canvases = []
        self.ani = []
        for i in range(self.num_graphs):
            self.figures.append(plt.Figure(figsize=(5, 2)))
            self.vis_args[i].append(self.terminal)
            self.vis.append(self.vis_types[i](self.figures[-1], 1, 1, 1, self.vis_args[i]))
            self.canvases.append(FigureCanvasTkAgg(self.figures[-1], master=self))
            self.canvases[-1].get_tk_widget().grid(column=(i // self.graph_columns), row=(i % self.graph_columns))
            self.ani.append(self.vis[-1].init_animation())