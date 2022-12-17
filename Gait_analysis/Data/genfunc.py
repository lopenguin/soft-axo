import numpy as np
import csv

WFILE = r"func.csv"
func = lambda x : np.sin(x)

MEASURES = [lambda x : x[-1],
            np.mean,
            np.std,
            np.median,
            np.max,
            np.min]

with open(WFILE, 'w') as file:
    writer = csv.writer(file, lineterminator='\n')
    L = 100
    buff = []
    for x in np.linspace(0, 1, 10000):
        if len(buff) < L:
            buff.append(func(x))
        else:
            buff = buff[1:] + [func(x)]
            towrite = [measure(buff) for measure in MEASURES]
            writer.writerow([x] + [towrite[0]] + [x])

print('Done')