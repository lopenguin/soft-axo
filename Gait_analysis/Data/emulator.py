import numpy as np
import csv

class Emulator:

    """
    Initialization function for feature generation class.

    Args:
        funcs       : [func(x)], custom features where x array with 1 <= |x| <= window_size
        window_size : the time period (in seconds) of data to apply functions to
        labels      : [time, gait percentage]
        file_in     : data file
        attributes  : which attributes in file_in to write into features
        file_out    : feature file to write to
    """
    def __init__(self, funcs, window_size, labels,
                 file_in, attributes, file_out):
        self.funcs = funcs
        self.window_size = window_size
        self.labels = labels
        self.file_in = file_in
        self.attributes = attributes
        self.file_out = file_out

    
    def write(self):
        with open(self.file_in) as rfile:
            reader = csv.reader(rfile)
            with open(self.file_out, 'w') as wfile:
                writer = csv.writer(wfile)
                idxs, firstrow = {}, True
                for row in reader:
                    if firstrow:
                        for i, attr in enumerate(row):
                            if attr in self.attributes:
                                idxs[i] = []
                        firstrow = False
                    else:
                        towrite = []
                        for i in idxs:
                            if len(idxs[i]) > 1 and idxs[0][-1] - idxs[0][0] > self.window_size:
                                idxs[i] = idxs[i][1:] + [float(row[i])]
                            else:
                                idxs[i].append(float(row[i]))
                            # print(idxs)
                            if i != 0:
                                for func in self.funcs:
                                    towrite.append(func(idxs[i]))

                        writer.writerow([row[0]] + towrite)




