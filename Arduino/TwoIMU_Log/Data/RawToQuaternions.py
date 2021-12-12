'''
Converts binary time-quaternion save file into usable times, quaternion, and euler angle.

UNTESTED!!

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
QUATERNION_SAVE_FILE = RAW_DATA_FILE[:-4] + "_quaternions.csv"
EULER_SAVE_FILE = RAW_DATA_FILE[:-4] + "_euler.csv"
#---------------------------------------------------------------------------------------

# sourced from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/
def euler_from_quaternion(q):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise) <- what we want for foot
        yaw is rotation around z in radians (counterclockwise)
        """
        x = q[0]
        y = q[1]
        z = q[2]
        w = q[3]
        t0 = +2.0 * (w * x + y * z)
        t1 = +1.0 - 2.0 * (x * x + y * y)
        roll_x = math.atan2(t0, t1)

        t2 = +2.0 * (w * y - z * x)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)

        t3 = +2.0 * (w * z + x * y)
        t4 = +1.0 - 2.0 * (y * y + z * z)
        yaw_z = math.atan2(t3, t4)

        return roll_x, pitch_y, yaw_z # in radians

# begin script
with open(EULER_SAVE_FILE, mode = 'w') as walking_data:
    with open(QUATERNION_SAVE_FILE, mode = 'w') as quaternion_data:
        with open(RAW_DATA_FILE, mode='rb') as raw_data:
            # just setting up csv reader/writer objects...
            euler_writer = csv.writer(walking_data, delimiter = ',', lineterminator = '\n')
            quaternion_writer = csv.writer(quaternion_data, delimiter = ',', lineterminator = '\n')

            # simple counters
            line_counter = 0
            lastTime = 0;

            # read a quaternion & time each time (time: 1 byte; quat: 4 components, 8 bytes)
            line = raw_data.read(9)
            while (len(line) != 0):
                row = []
                line_counter = line_counter + 1
                # times
                row.append(float(line[0])/10 + lastTime); # convert to milliseconds
                lastTime = row[0];

                # quaternions
                for index in range(1,8,2):
                    num = (line[index] << 8) | line[index+1]
                    # convert this num to the quaternion
                    num = num / 10000
                    num = round(num - 1, 4)
                    row.append(num)
                quaternion_writer.writerow(row)
                euler_list = list(map(str, euler_from_quaternion(row[1:5])))
                euler_writer.writerow([row[0], euler_list[0], euler_list[1], euler_list[2]])
                line = raw_data.read(9)

        # output stuff
        print('---')
        print(f'Processed {line_counter} lines.')
        print(f'Saved quaternion data to {pathlib.Path().resolve()}\{QUATERNION_SAVE_FILE}')
        print(f'Saved euler data to {pathlib.Path().resolve()}\{EULER_SAVE_FILE}')
        print('---')
