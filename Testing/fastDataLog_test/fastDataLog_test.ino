/*
Testing data logging in *fast mode*

That means:
- store data to write in large buffer
- write data in large batch using separate threads

Lorenzo Shaikewitz, 8/4/2021
*/

#include <NXPMotionSense_Lorenzo.h>
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

const short BUTTON_PIN{23};

const int FLASH_CS_PIN{ 6 };
const char FILE_NAME[15]{"walk1.csv"};
const int FILE_SIZE{ 840000 };    // Lasts 5 minutes of data... also quite large.
const int BUF_SIZE{1400};    // should be multiple of 28 (FIX TO 256 at a time)
int lastPos{ 0 };

NXPMotionSense imu;
NXPSensorFusion filter;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    while (digitalRead(BUTTON_PIN) != LOW) {
        delay(50);
    }

    // IMU setup
    imu.begin();
    filter.begin(100);  // use this to change the update rate (in Hz)

    // SerialFlash setup
    if (!SerialFlash.begin(FLASH_CS_PIN)) {
        Serial.println("Unable to access SPI Flash chip");
        while (1);
    }

    if (!SerialFlash.exists(FILE_NAME)) {
        Serial.println("File not found... created new one.");
    } else {
        Serial.println("File already exists. Make sure to erase flash regularly.");
        SerialFlash.remove(FILE_NAME);
    }
    if (!SerialFlash.create(FILE_NAME, FILE_SIZE)) {
        Serial.println("Not enough space! Time to erase your flash.");
        while (1) {
            // blink LED
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
        }
    }
}

void loop() {
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float quat[4];

    char buf[BUF_SIZE]{};
    int bufIndex{0};

    while (bufIndex < BUF_SIZE) {
        if (imu.available()) {
            // Read the motion sensors
            imu.readMotionSensor(ax, ay, az, gx, gy, gz, mx, my, mz);

            // Update the SensorFusion filter
            filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

            filter.getQuaternion(quat);
            addQuatToBuf(quat, buf, bufIndex);
        }
    }

    SerialFlashFile file = SerialFlash.open(FILE_NAME);
    file.seek(lastPos);
    if (!file.write(buf, BUF_SIZE)) {
        file.close();
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("File size exceeded!");
        while (1);
    }
    Serial.println(buf);
    lastPos = file.position();
    file.flush();
    bufIndex = 0;
}

void addQuatToBuf(float* quat, char* buf, int& bufIndex) {
    // each quat is 6 chars: +x.xxxx
    // separated by ','
    // line ends in '\n'
    // total: 24 chars
    for (int i{ 0 }; i < 4; ++i) {
        String q = String(quat[i], 4);
        const char* q_cStr = q.c_str();
        // Serial.print(q_cStr);
        // Serial.print(" : ");
        // Serial.println(q_cStr[0]);
        if (q_cStr[0] == '-') {
            for (int j{ 0 }; j < 6; ++j) {
                buf[bufIndex] = q_cStr[j];
                ++bufIndex;
            }
        } else {
            buf[bufIndex] = '0';
            ++bufIndex;
            for (int j{ 0 }; j < 5; ++j) {
                buf[bufIndex] = q_cStr[j];
                ++bufIndex;
            }
        }
        if (i != 3)
            buf[bufIndex] = ',';
        else
            buf[bufIndex] = '\n';
        ++bufIndex;
    }
}
