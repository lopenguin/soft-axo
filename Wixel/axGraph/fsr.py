"""
Converts FSR data to times at which a step was detected (in seconds)
"""

import csv

RATE = 100 # Hz

FSR_FILE = r"C:\Users\janwa\OneDrive\Documents\soft-axo\Wixel\axGraph\Axo0_2022-05-06_163215_SAMPLE_TESTING_FSR.csv"

OUTPUT_FILE = r"steps.csv"

with open(FSR_FILE) as ffile:
    with open(OUTPUT_FILE, 'w') as wfile:
        reader = csv.reader(ffile, delimiter=',')
        writer = csv.writer(wfile,delimiter=',',lineterminator='\n')
        i = 0
        step = False
        for value in reader:
            if float(value[0]) > 500 and not step:
                step = True
                writer.writerow([i * 1 / RATE])
            elif float(value[0]) < 100:
                step = False
            i += 1