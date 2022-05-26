import matplotlib.pyplot as plt
import csv
import numpy as np

NAME = r'C:\Users\janwa\OneDrive\Documents\soft-axo\SerialGUI\axRead\data.csv'

l = []
x = []

min = 13000
max = 15000
i = 0

with open(NAME) as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        i += 1
        if i > max or i < min:
            continue
        if len(row) > 1 and row[0] == 'L ':
            x.append(float(row[1]))
            l.append(float(row[2]))

print('done')

plt.plot(x,l)
plt.gca().get_xaxis().set_ticks(np.linspace(np.min(x),np.max(x)))
plt.gca().get_yaxis().set_ticks(np.linspace(np.min(l),np.max(l)))
plt.show()
