import csv
DATA_IN = r"data_L .csv"
DATA_OUT = r"data_L_good.csv"

with open(DATA_IN) as csvfile_in:
    with open(DATA_OUT, 'w') as csvfile_out:
        reader = csv.reader(csvfile_in)
        writer = csv.writer(csvfile_out, lineterminator='\n')

        for row in reader:
            if float(row[1]) < 1024 and float(row[1]) > -1 and float(row[0]) > 10**4 and float(row[0]) < 10**6:
                writer.writerow(row)
    