import matplotlib.pyplot as plt
import csv
import numpy as np

NAME = '/Users/Neil/Documents/GitHub/soft-axo/SerialGUI/axRead/data.csv'

l = []
x = []

min = 10000
max = 11000
i = 0

with open(NAME) as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        i += 1
        if i > max or i < min:
            continue
        if len(row) > 1 and row[0] == 'L ':
            x.append(row[1])
            l.append(row[2])

print('done')

plt.scatter(x,l)
plt.show()
