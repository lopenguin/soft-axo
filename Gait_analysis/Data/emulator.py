import csv
from tqdm import tqdm
import numpy as np

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
        # print(self.labels[1])

    
    def write(self):
        towrite = []
        with open(self.file_in) as rfile:
            reader = csv.reader(rfile)
            with open(self.file_out, 'w') as wfile:
                writer = csv.writer(wfile)
                idxs, firstrow = {}, True
                k = 0
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
                                # print('(' + str(idxs[0][-1]) + ', ' + str(idxs[0][0]) + ')')
                                idxs[i] = idxs[i][1:] + [float(row[i])]
                            else:
                                idxs[i].append(float(row[i]))
                            # print(len(idxs[i]))
                            # print(idxs)
                            if i != 0:
                                for func in self.funcs:
                                    towrite.append(func(idxs[i]))
                                # towrite = towrite + idxs[i][-15:]
                                # print('--')

                        if k >= len(self.labels[1]):
                            break
                        writer.writerow([row[0]] + towrite + [self.labels[1][k]])
                        k += 1

        print(str(len(towrite)) + " features written")
        



