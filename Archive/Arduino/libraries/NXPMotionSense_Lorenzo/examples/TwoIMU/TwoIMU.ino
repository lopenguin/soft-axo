// Full orientation sensing using NXP's advanced sensor fusion algorithm.
//
// You *must* perform a magnetic calibration before this code will work.
//
// To view this data, use the Arduino Serial Monitor to watch the
// scrolling angles, or run the OrientationVisualiser example in Processing.
// update rate: 100 Hz (see datasheet to config for up to 800 Hz!)
//
// Modified for use with Adafruit NXP board + Teensy Prop Shield

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

NXPMotionSense imu_prop(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0);
NXPSensorFusion filter_prop;

NXPMotionSense imu_ada(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1);
NXPSensorFusion filter_ada;

unsigned long startTime{};
bool adaRead{0};

void setup() {
  Serial.begin(9600);
  imu_prop.begin();
  imu_ada.begin();

  filter_prop.begin(100);
  filter_ada.begin(100);

  startTime = micros();
}

struct SpaceXYZ {
  float x{};
  float y{};
  float z{};
};

void loop() {
  SpaceXYZ acc_prop{};
  SpaceXYZ gyr_prop{};
  SpaceXYZ mag_prop{};

  SpaceXYZ acc_ada{};
  SpaceXYZ gyr_ada{};
  SpaceXYZ mag_ada{};

  float roll_prop, pitch_prop, heading_prop;
  float roll_ada, pitch_ada, heading_ada;

  // TODO:
  // - Double data rate to 200 Hz

  if (adaRead) {
      if (imu_prop.available()) {
        adaRead = 0;
        Serial.println(micros() - startTime);
        startTime = micros();
        // Read the motion sensors
        imu_prop.readMotionSensor(acc_prop.x, acc_prop.y, acc_prop.z, gyr_prop.x, gyr_prop.y, gyr_prop.z, mag_prop.x, mag_prop.y, mag_prop.z);

        // Update the SensorFusion filter
        filter_prop.update(gyr_prop.x, gyr_prop.y, gyr_prop.z, acc_prop.x, acc_prop.y, acc_prop.z, mag_prop.x, mag_prop.y, mag_prop.z);

        // print the heading, pitch and roll
        roll_prop = filter_prop.getRoll();
        pitch_prop = filter_prop.getPitch();
        heading_prop = filter_prop.getYaw();
        Serial.print("Prop: ");
        Serial.print(heading_prop);
        Serial.print(", ");
        Serial.print(pitch_prop);
        Serial.print(", ");
        Serial.print(roll_prop);
        Serial.print('\n');

      }
  }

  if (!adaRead) {
      if (imu_ada.available()) {
          adaRead = 1;
          // Read the motion sensors
          imu_ada.readMotionSensor(acc_ada.x, acc_ada.y, acc_ada.z, gyr_ada.x, gyr_ada.y, gyr_ada.z, mag_ada.x, mag_ada.y, mag_ada.z);

          // Update the SensorFusion filter
          filter_ada.update(gyr_ada.x, gyr_ada.y, gyr_ada.z, acc_ada.x, acc_ada.y, acc_ada.z, mag_ada.x, mag_ada.y, mag_ada.z);

          // print the heading, pitch and roll
          roll_ada = filter_ada.getRoll();
          pitch_ada = filter_ada.getPitch();
          heading_ada = filter_ada.getYaw();
          Serial.print("Ada: ");
          Serial.print(heading_ada);
          Serial.print(", ");
          Serial.print(pitch_ada);
          Serial.print(", ");
          Serial.println(roll_ada);
      }
  }

}
