from stepper import Stepper
import numpy as np
import csv

DATA_IN = r'../Data/AB06/fsr.csv'
DATA_SUPP = r'../Neural_net/pdata.csv'
DATA_OUT = r'processed.csv'
ACCURACY = 10

with open(DATA_IN) as file:
    times, percents = [], []
    reader = csv.reader(file)
    for row in reader:
        if row[0] != 'Header':
            times.append(float(row[0]))
            percents.append(float(row[1]))
    supp_data = []
    with open(DATA_SUPP) as sfile:
        sreader = csv.reader(sfile)
        for row in sreader:
            supp_data.append(row[:-1])

    steps = Stepper()
    start, ret = steps.spin(times, percents)
    with open(DATA_OUT, 'w') as wfile:
        writer = csv.writer(wfile, lineterminator='\n')
        for i, (t, sd, y, yp) in enumerate(zip(times, supp_data, percents, ret)):
            if i >= start:
                writer.writerow(sd + [1 if ACCURACY < abs(y - yp) else 0])


    import sys
    sys.path.append('../util')
    import stats
    # for y, yp in zip(percents[start:], ret):
    #     print(y, yp)
    stats.get_stats(percents[start:], ret)

