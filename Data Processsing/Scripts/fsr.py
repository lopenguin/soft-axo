"""
Converts FSR data to times at which a step was detected (in seconds)
"""

import csv, sys

RATE = 100 # Hz

FSR_FILE = sys.argv[1]
OUTPUT_FILE = sys.argv[2][:-1]

with open(FSR_FILE) as ffile:
    with open(OUTPUT_FILE, 'w') as wfile:
        reader = csv.reader(ffile, delimiter=',')
        writer = csv.writer(wfile,delimiter=',',lineterminator='\n')
        i = 0
        step = False
        for value in reader:
            if float(value[1]) < 1000 and float(value[1]) > 350 and not step:
                step = True
                if float(value[0]) < 10**7:
                    writer.writerow([value[0]])
            elif float(value[1]) < 100:
                step = False
            i += 1