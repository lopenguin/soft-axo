'''
Converts binary time-FSR save file into usable times and FSR readings

Lorenzo Shaikewitz, 10/4/2021
Original code by Neil J
'''
import csv
import string
import pathlib
import math

#----------Change these file names to fit-----------------------------------------------
RAW_DATA_FILE = input("Data file path: ")

# cut off file extension
SAVE_FILE = RAW_DATA_FILE[:-4] + "_fsr.csv"
#---------------------------------------------------------------------------------------

# begin script
with open(SAVE_FILE, mode = 'w') as walking_data:
    with open(RAW_DATA_FILE, mode='rb') as raw_data:
        # just setting up csv reader/writer objects...
        fsr_writer = csv.writer(walking_data, delimiter = ',', lineterminator = '\n')

        # simple counters
        line_counter = 0
        lastTime = 0;

        # read a time & fsr (time: 1 byte; fsr: 1 byte)
        line = raw_data.read(2)
        while (len(line) != 0):
            row = []
            line_counter = line_counter + 1
            # times (0 corresponds to 800)
            row.append(float(line[0])*100 + 800 + lastTime); # convert to milliseconds
            lastTime = row[0]

            # fsr
            num = line[1]
            row.append(num)
            fsr_writer.writerow(row)
            line = raw_data.read(2)

        # output stuff
        print('---')
        print(f'Processed {line_counter} lines.')
        print(f'Saved FSR data to {pathlib.Path().resolve()}\{SAVE_FILE}')
        print('---')
