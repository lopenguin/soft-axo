"""
Processes the Wixel data into files of specified data types.
"""

import csv

def split(s):
    ret = []
    j = 0
    buffer = ''
    while j < len(s):
        if j + 2 < len(s) and ((s[j:j + 3] == '),(') or (s[j:j + 3] == ', (')):
            ret.append(buffer)
            buffer = ''
            j += 3
        elif j + 1 < len(s) and ((s[j:j + 2] == ',(' or s[j:j + 2] == ')\n')):
            ret.append(buffer)
            buffer = ''
            j += 2
        elif s[j] == ',' or s[j] == ')' or s[j] == '\n':
            ret.append(buffer)
            buffer = ''
            j += 1
        else:
            if (s[j] != ' ' or s[j] != '\n'):
                buffer += s[j]
            j += 1
    return ret

FILENAME = r"C:\Users\janwa\OneDrive\Documents\axGraph\Axo0_2022-05-06_163215_SAMPLE_TESTING.log"

OUTPUT = [('FSR',2),
          ('LOAD',2),
          ('IMU', 23)] # name, total number of datapoints per entry

OUTPUT_FILES = []
for (file, _) in OUTPUT:
    OUTPUT_FILES.append(FILENAME[:-4] + "_" + file + ".csv")

with open(FILENAME, newline='\n') as csvfile:
    data = {}
    for (file, _) in OUTPUT:
        data[file] = []
    
    skipped = 0
    total = 0
    for row in csvfile:
        info = split(row)
        for (type, length) in OUTPUT:
            if (len(info) == length and info[0] == type):
                total += 1
                try:
                    for num in info[1:]:
                        float(num)
                    data[type].append(info[1:])
                    break
                except:
                    print("Error encountered, check if", num, "is float. Came from '" + row[:-1] + "'. Skipping datapoint...")
                    skipped += 1
                    break
    
    for i in range(len(OUTPUT_FILES)):
        with open(OUTPUT_FILES[i],'w') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',lineterminator='\n')
            for elem in data[OUTPUT[i][0]]:
                writer.writerow(elem)
    
    print("\nSkipped " + str(skipped / total) + "%" + " of all datapoints ->", skipped, "total datapoints during parsing.")

        
    