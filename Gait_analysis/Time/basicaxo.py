from stepper import Stepper
import numpy as np
import csv

DATA_IN = r'../Data/AB06/fsr.csv'


with open(DATA_IN) as file:
    times, percents = [], []
    reader = csv.reader(file)
    for row in reader:
        if row[0] != 'Header':
            times.append(float(row[0]))
            percents.append(float(row[1]))

    steps = Stepper()
    start, ret = steps.spin(times, percents)

    import sys
    sys.path.append('../util')
    import stats
    # for y, yp in zip(percents[start:], ret):
    #     print(y, yp)
    stats.get_stats(percents[start:], ret)
