/*
Tests BNO055 pair with ankle exo. Designed for use with two BNO055's and ankle
exo driver board. Data can be printed to the serial monitor or transmitted via
Wixel.

Output is TODO

TODO: TEST!!
TODO: Write calibration routine--see https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/device-calibration

Lorenzo Shaikewitz, 4/17/2022
*/
#include <Metro.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// use Serial for printing to the monitor and Serial1 for wireless with Wixel.
#define SerialOut Serial
// #define SerialOut Serial1

// Constants
uint16_t SAMPLERATE{100}; // Hz
adafruit_bno055_offsets_t shinOffsets{};
adafruit_bno055_offsets_t footOffsets{};

// IMU data structure
struct IMUData {
    imu::Quaternion quat{};
    sensors_event_t angVel{};
    sensors_event_t linAccel{};
    sensors_event_t accel{};
    sensors_event_t grav{};
    uint8_t cal{};
};

// Other variables
Metro IMUtimer = Metro(SAMPLERATE); // read the IMUs at SAMPLERATE Hz
IMUData shinData;
IMUData footData;

// Sensor Objects (note: check ID)
Adafruit_BNO055 shinIMU = Adafruit_BNO055(BNO055_ID, 0x29);
Adafruit_BNO055 footIMU = Adafruit_BNO055(BNO055_ID, 0x28);


void setup() {
    SerialOut.begin(57600);
    SerialOut.println("IMU Test\n");

    // initialize sensors
    bool shinIMUStarted = shinIMU.begin();
    bool footIMUStarted = footIMU.begin();

    if (!shinIMUStarted) {
        SerialOut.println("Error connecting to shin IMU. Foot IMU status unknown.");
        while (1);
    }
    if (!footIMUStarted) {
        SerialOut.println("Error connecting to foot IMU. Shin IMU successfully connected.");
        while (1);
    }

    delay(1000);

    // set up external crystal for optimal performance
    shinIMU.setExtCrystalUse(true);
    footIMU.setExtCrystalUse(true);

    // Switch to NDOF mode--use all 9 DOFs, 100 Hz fusion data
    shinIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
    footIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);

    // Calibration setup
    shinIMU.setSensorOffsets(shinOffsets);
    footIMU.setSensorOffsets(footOffsets);

    // one more delay just to be safe
    delay(1000);
}

void loop() {
    if (IMUtimer.check()) {
        // Data format:
        // Shin, Foot
        // Calibration status, Quat (w,x,y,z), Ang vel (x, y, z), linear accel (x, y, z), accel (x, y, z), grav (x, y, z)

        // get data from shin IMU
        uint8_t gyr, acc, mag; // placeholder vars
        shinIMU.getCalibration(&shinData.cal, &gyr, &acc, &mag);
        shinData.quat = shinIMU.getQuat();
        shinIMU.getEvent(&shinData.angVel, Adafruit_BNO055::VECTOR_GYROSCOPE);
        shinIMU.getEvent(&shinData.linAccel, Adafruit_BNO055::VECTOR_LINEARACCEL);
        shinIMU.getEvent(&shinData.accel, Adafruit_BNO055::VECTOR_ACCELEROMETER);
        shinIMU.getEvent(&shinData.grav, Adafruit_BNO055::VECTOR_GRAVITY);

        // get data from foot IMU
        footIMU.getCalibration(&footData.cal, &gyr, &acc, &mag);
        footData.quat = footIMU.getQuat();
        footIMU.getEvent(&footData.angVel, Adafruit_BNO055::VECTOR_GYROSCOPE);
        footIMU.getEvent(&footData.linAccel, Adafruit_BNO055::VECTOR_LINEARACCEL);
        footIMU.getEvent(&footData.accel, Adafruit_BNO055::VECTOR_ACCELEROMETER);
        footIMU.getEvent(&footData.grav, Adafruit_BNO055::VECTOR_GRAVITY);

        printIMUData();
    }
}

void printIMUData() {
    // Calibration status, Quat (w,x,y,z), Ang vel (x, y, z), linear accel (x, y, z), accel (x, y, z), grav (x, y, z)

    // shin
    SerialOut.printf("IMU, %u, (%4.4f, %4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f), ,",
                     shinData.cal,
                     shinData.quat.w(), shinData.quat.x(), shinData.quat.y(), shinData.quat.z(),
                     shinData.angVel.gyro.x, shinData.angVel.gyro.y, shinData.angVel.gyro.z,
                     shinData.linAccel.acceleration.x, shinData.linAccel.acceleration.y, shinData.linAccel.acceleration.z,
                     shinData.accel.acceleration.x, shinData.accel.acceleration.y, shinData.accel.acceleration.z,
                     shinData.grav.acceleration.x, shinData.grav.acceleration.y, shinData.grav.acceleration.z);

    // foot
    SerialOut.printf("%u, (%4.4f, %4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f), (%4.4f, %4.4f, %4.4f)\n",
                     footData.cal,
                     footData.quat.w(), footData.quat.x(), footData.quat.y(), footData.quat.z(),
                     footData.angVel.gyro.x, footData.angVel.gyro.y, footData.angVel.gyro.z,
                     footData.linAccel.acceleration.x, footData.linAccel.acceleration.y, footData.linAccel.acceleration.z,
                     footData.accel.acceleration.x, footData.accel.acceleration.y, footData.accel.acceleration.z,
                     footData.grav.acceleration.x, footData.grav.acceleration.y, footData.grav.acceleration.z);
}
