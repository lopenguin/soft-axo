/*
Calibrate the BNO055 IMUs.

To operate:
1. Select I2C address of IMU you are using
2. Perform accelerometer calibration:
    Place the device in 6 stationary positions for a few seconds each.
    Make sure there is slow movement between the positions
    +X, -X, +Y, -Y, +Z, -Z
3. Perform gyroscope calibration:
    Place the device in a single stable position for a few seconds.
4. Perform magnetometer calibration:
    Make random movements (like writing the letter 8) in the air.

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

// Constants and timer
uint16_t SAMPLERATE{100}; // Hz
Metro IMUtimer = Metro(SAMPLERATE); // read the IMUs at SAMPLERATE Hz

// Sensor Object
// Adafruit_BNO055 myIMU = Adafruit_BNO055(BNO055_ID, 0x29); // shin
Adafruit_BNO055 myIMU = Adafruit_BNO055(BNO055_ID, 0x28); // foot

void setup() {
    SerialOut.begin(57600);
    SerialOut.println("IMU Calibration!\n");

    // initialize sensor
    bool imuStarted = myIMU.begin();
    if (!imuStarted) {
        SerialOut.println("Error connecting to IMU.");
        while (1);
    }

    delay(1000);

    // set up external crystal for optimal performance
    myIMU.setExtCrystalUse(true);

    // Calibration occurs in the main mode
    myIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
}

void loop() {
    if (IMUtimer.check()) {
        uint8_t sys, gyr, acc, mag;
        myIMU.getCalibration(&sys, &gyr, &acc, &mag);

        SerialOut.printf("System: %d, Gyro: %d, Accel: %d, Mag: %d\n",
                            sys, gyr, acc, mag);

        // check if the system is well calibrated
        if ((sys == 3 && gyr == 3) && (acc == 3 && mag == 3)) {
            SerialOut.println("---");
            SerialOut.println("Calibration Complete!");

            // write the offsets
            adafruit_bno055_offsets_t offsets{};
            myIMU.getSensorOffsets(offsets);

            // write out the offset code
            SerialOut.printf("adafruit_bno055_offsets_t offsets{\n"
                "    %d, %d, %d, // accel offsets\n"
                "    %d, %d, %d, // mag offsets\n"
                "    %d, %d, %d, // gyro offsets\n"
                "    %d,         // accel radius\n"
                "    %d          // mag radius\n"
                "};\n",
                offsets.accel_offset_x, offsets.accel_offset_y, offsets.accel_offset_z,
                offsets.mag_offset_x, offsets.mag_offset_y, offsets.mag_offset_z,
                offsets.gyro_offset_x, offsets.gyro_offset_y, offsets.gyro_offset_z,
                offsets.accel_radius,
                offsets.mag_radius);

            // pause.. calibration is complete.
            while (1) {
                if (Serial.available())
                    break;
            }
        }
    }
}
