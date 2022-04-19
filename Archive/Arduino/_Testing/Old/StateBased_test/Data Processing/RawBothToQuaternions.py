import csv
import string
import pathlib
import math

#----------Change these file names to fit-----------------------------------------------
RAW_DATA_FILE = input("Data file path: ")

# cut off file extension
FULL_QUATERNION_SAVE_FILE = RAW_DATA_FILE[:-4] + "_full_quaternions.csv"
REL_QUATERNION_SAVE_FILE = RAW_DATA_FILE[:-4] + "_rel_quaternions.csv"
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


'''
gets the quaternion representating rotation by q2, then q1*
This gives a relative angle. ORDER MATTERS.
- Lorenzo Shaikewitz
'''
def getRelQuat(q1, q2):
    relQuat = []
    relQuat[0] = q1[0]*q2[0] - (-q1[1])*q2[1] - (-q1[2])*q2[2] - (-q1[3])*q2[3];

    relQuat[1] = q1[0]*q2[1] + (-q1[1])*q2[0] + (-q1[2])*q2[3] - (-q1[3])*q2[2];

    relQuat[2] = q1[0]*q2[2] - (-q1[1])*q2[3] + (-q1[2])*q2[0] + (-q1[3])*q2[1];

    relQuat[3] = q1[0]*q2[3] + (-q1[1])*q2[2] - (-q1[2])*q2[1] + (-q1[3])*q2[0];


# begin script
with open(EULER_SAVE_FILE, mode = 'w') as walking_data:
    with open(FULL_QUATERNION_SAVE_FILE, mode = 'w') as full_quaternion_data:
        with open(REL_QUATERNION_SAVE_FILE, mode = 'w') as rel_quaternion_data:
            with open(RAW_DATA_FILE, mode='rb') as raw_data:
                # just setting up csv reader/writer objects...
                euler_writer = csv.writer(walking_data, delimiter = ',', lineterminator = '\n')
                full_quaternion_writer = csv.writer(full_quaternion_data, delimiter = ',', lineterminator = '\n')
                rel_quaternion_writer = csv.writer(rel_quaternion_data, delimiter = ',', lineterminator = '\n')

                # simple counters
                line_counter = 0

                # read 2 quaternions each time (8 components, 16 bytes)
                line = raw_data.read(16)
                while (len(line) != 0):
                    row = []
                    line_counter = line_counter + 1
                    for index in range(0,15,2):
                        num = (line[index] << 8) | line[index+1]
                        # convert this num to the quaternion
                        num = num / 10000
                        num = round(num - 1, 4)
                        row.append(num)
                    full_quaternion_writer.writerow(row)
                    euler_writer.writerow(list(map(str, euler_from_quaternion(row[0:4]) + euler_from_quaternion(row[4:8]))))
                    rel_quaternion_writer.writerow(list(map(str,getRelQuat(row[0:4], row[4:8]))))
                    line = raw_data.read(16)

            # output stuff
            print('---')
            print(f'Processed {line_counter} lines.')
            print(f'Saved quaternion data to {pathlib.Path().resolve()}\{FULL_QUATERNION_SAVE_FILE}')
            print(f'Saved euler data to {pathlib.Path().resolve()}\{EULER_SAVE_FILE}')
            print('---')
