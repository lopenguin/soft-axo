'''
Converts binary data file to a CSV that can be read by MATLAB.
'''

import csv
import pathlib
import math

#----------Change these file names to fit-----------------------------------------------
RAW_DATA_FILE = input("Data file path: ")

# cut off file extension
SAVE_FILE = RAW_DATA_FILE[:-4] + ".csv"
#---------------------------------------------------------------------------------------

# begin script
with open(SAVE_FILE, mode = 'w') as out:
    with open(RAW_DATA_FILE, mode='rb') as raw:
        # set up writer
        writer = csv.writer(out, delimiter = ',', lineterminator = '\n')

        # read the raw data
        


