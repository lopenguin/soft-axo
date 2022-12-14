import numpy as np
import csv

WFILE = "func.csv"
f = lambda x : 0.5 * np.sin(x) + 1
df = lambda x : 0.5 * np.cos(x) + 1
d2f = lambda x : -0.5 * np.sin(x) + 1

with open(WFILE, 'w') as file:
    writer = csv.writer(file, lineterminator='\n')
    for x in np.linspace(0, 1, 10000):
        writer.writerow([f(x), df(x), d2f(x), x])

print('Done')