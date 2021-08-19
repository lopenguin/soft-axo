// Full orientation sensing using NXP's advanced sensor fusion algorithm.
//
// You *must* perform a magnetic calibration before this code will work.
//
// To view this data, use the Arduino Serial Monitor to watch the
// scrolling angles, or run the OrientationVisualiser example in Processing.
// update rate: 100 Hz (see datasheet to config for up to 800 Hz!)

#include <NXPMotionSense_Lorenzo.h>
#include <Wire.h>
#include <EEPROM.h>

NXPMotionSense imu;
NXPSensorFusion filter;

void setup() {
  Serial.begin(9600);
  imu.begin();
  filter.begin(200);
}

void loop() {
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float roll, pitch, heading;

    if (imu.available()) {
//        unsigned long startTime{ millis() };

        // Read the motion sensors
        imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

        // Update the SensorFusion filter
        filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);
//        Serial.println(millis() - startTime);

        // print the heading, pitch and roll
        roll = filter.getRoll();
        pitch = filter.getPitch();
        heading = filter.getYaw();
//        Serial.print("Orientation: ");
//        Serial.print(heading);
//        Serial.print(", ");
//        Serial.print(pitch);
//        Serial.print(", ");
//        Serial.println(roll);

        Serial.print("Heading ");
        Serial.print(heading);
        Serial.print(", ");
        Serial.print("Pitch ");
        Serial.print(pitch);
        Serial.print(", ");
        Serial.print("Roll ");
        Serial.println(roll);

//        float w, x, y, z;
//        filter.getQuaternion(w, x, y, z);
//        Serial.print("Quaternion: ");
//        Serial.print(w);
//        Serial.print(", ");
//        Serial.print(x);
//        Serial.print(", ");
//        Serial.print(y);
//        Serial.print(", ");
//        Serial.println(z);
    }
}
