import matplotlib.pyplot as plt
import csv

START = 0
LENGTH = 5000
END = START + LENGTH

data = [[], [], [], [], []]
print(data)
with open('data.csv') as file:
    reader = csv.reader(file)
    for row in reader:
        for i, elem in enumerate(row):
            data[i].append(float(elem))

# print(data[1][START:LENGTH])

plt.plot(data[0][START:END], data[1][START:LENGTH])
plt.plot(data[0][START:END], data[2][START:LENGTH])
plt.plot(data[0][START:END], data[3][START:LENGTH])
plt.plot(data[0][START:END], data[4][START:LENGTH])
plt.show()
