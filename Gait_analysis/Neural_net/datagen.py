import csv
from random import uniform

DATA_FILE = r'simple_data.csv'
N = 6000

def classify(x, p1, p2, ctype='line'):
    if ctype == 'line':
        m = (p2[1] - p1[1]) / (p2[0] - p1[0])
        return int(x[1] >= m * x[0] - m * p1[0] + p1[1])
    elif ctype == 'add':
        return x[0] + x[1]


with open(DATA_FILE, 'w') as file:
    writer = csv.writer(file, delimiter=',',lineterminator='\n')
    p1, p2 = (uniform(-1, 1), uniform(-1, 1)), (uniform(-1, 1), uniform(-1, 1))
    for _ in range(N):
        x = (uniform(-1, 1), uniform(-1, 1))
        writer.writerow([x[0], x[1], classify(x, p1, p2, 'add')])