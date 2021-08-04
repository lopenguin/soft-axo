/*
Testing of the IMU with data logging onto flash memory.
Using IMU built in to Teensy Prop Shield at 100 Hz.

Lorenzo Shaikewitz, 7/31/2021
*/

#include <NXPMotionSense_Lorenzo.h>
#include <Wire.h>
#include <EEPROM.h>

// for flash storage
#include <SPI.h>
#include <SdFat_Adafruit.h>
#include <Adafruit_SPIFlash.h>

const short FLASH_CS_PIN{6};
const char FILENAME[10]{"Test.csv"};

// IMU class objects
NXPMotionSense imu;
NXPSensorFusion filter;

// Flash class objects
Adafruit_FlashTransport_SPI flashTransport(FLASH_CS_PIN, SPI);
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;

void setup() {
    Serial.begin(115200);
    imu.begin();
    filter.begin(100);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // wait for serial monitor so we don't overwrite files.
    while (!Serial) {
        delay(10);
    }

    /* Flash setup */
    // Startup chip
    if (!flash.begin()) {
        Serial.println("Failed to initialize flash. Freezing...");
        while (1);
    }
    // Check filesystem.
    if (!fatfs.begin(&flash)) {
        Serial.println("Error, failed to mount newly formatted filesystem!");
        Serial.println("Was the flash chip formatted?");
        while(1);
    }
    // turn off LED writing for indicator
    flash.setIndicator(LED_BUILTIN, false);
    digitalWrite(LED_BUILTIN, LOW);

    // remove file if exists.
    if (fatfs.exists(FILENAME)) {
        fatfs.remove(FILENAME);
    }
    File dataFile = fatfs.open(FILENAME, FILE_WRITE);
    if (dataFile) {
        dataFile.print(millis());
    }
}

void loop() {
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float roll, pitch, heading;

    if (imu.available()) {
        unsigned long startTime{ millis() };
        // Read the motion sensors
        imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

        // Update the SensorFusion filter
        filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

        float quat[4]{};
        filter.getQuaternion(quat);

        // Serial.print(quat[0]);
        // Serial.print(", ");
        // Serial.print(quat[1]);
        // Serial.print(", ");
        // Serial.print(quat[2]);
        // Serial.print(", ");
        // Serial.println(quat[3]);
        Serial.println(millis()- startTime);

        // open file and save data
        File dataFile = fatfs.open(FILENAME, FILE_WRITE);
        Serial.println(millis()- startTime);
        if (dataFile) {
            dataFile.print(quat[0], 4);
            dataFile.print(',');
            dataFile.print(quat[1], 4);
            dataFile.print(',');
            dataFile.print(quat[2], 4);
            dataFile.print(',');
            dataFile.print(quat[3], 4);
            //
            // dataFile.print(',');
            // dataFile.print(millis());
            dataFile.print('\n');
        } else {
            Serial.println("File write failure :(");
        }
        Serial.println(millis()- startTime);
        dataFile.close();
        Serial.println(millis()- startTime);
    }
}

String formatQuat(float* quat) {
    String s = String(quat[0], 4);
    s += ",";
    s += String(quat[1], 4);
    s += ",";
    s += String(quat[2], 4);
    s += ",";
    s += String(quat[3], 4);
    s += "\n";
    return s;
}
