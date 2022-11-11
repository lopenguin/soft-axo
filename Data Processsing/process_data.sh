#!/bin/bash
set -e
echo "Converting binary data..."
python3 Scripts/reader.py "${1}"
echo "Binary data converted to ASCII."

echo "Generating individual data files..."
python3 Scripts/preprocess.py "Data/data.csv"

echo "Transcribing IMU data..."
python3 Scripts/difference.py "Data/data.csv" "Data"
echo "IMU data done."

echo "Filtering FSR data into steps..."
python3 Scripts/fsr.py "Data/data_F .csv" "Data/steps.csv"
echo "FSR data done."

echo "Filtering load cell data to get rid of outliers..."
python3 Scripts/outliers.py "Data/data_I .csv" "Data/data_L_good.csv"
echo "Load cell data done."
echo ""

echo "Done :)"