import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation

class Visualization:
    def __init__(self, fig, nrows, ncols, index, animate=None, init=None):
        "Initializes visulatization class"
        raise NotImplemented()

    def init_animation(self):
        "To start the animation"
        raise NotImplemented()

    def default_init(self):
        "Initialize animation function (to be run once)"
        raise NotImplemented()

    def default_animate(self, i):
        "Animation callback function (to be run infinitely)"
        raise NotImplemented()

class SimpleVisualization(Visualization):
    def __init__(self, fig, nrows, ncols, index, args=[]):
        self.fig = fig
        self.x = np.linspace(0, 0, 100)
        self.ax = self.fig.add_subplot(nrows, ncols, index)
        self.line, = self.ax.plot([], [])

    def init_animation(self):
        # print('initialize animation...')
        self.ani = animation.FuncAnimation(self.fig, self.animate, np.arange(1, 100), interval=25, blit=True)
        return self.ani

    def init(self):
        self.line.set_data([], [])
        return self.line,

    def animate(self, i):
        # print("graphing")
        self.x = np.linspace(0, i / 10.0, 100)
        self.line.set_xdata(self.x)
        self.line.set_ydata(np.sin(self.x+ i /10.0))  # update the data
        self.ax.set_xlim(0, i / 10.0)
        self.ax.set_ylim(-2, 2)
        return self.line,