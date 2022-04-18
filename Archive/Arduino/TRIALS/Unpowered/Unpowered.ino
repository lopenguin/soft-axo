/*
Runs the Axo with motors inactive.

Lorenzo Shaikewitz, 2/18/2022
*/

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

#include <PWMServo_Lorenzo.h>

#include "Axo.h"
#include "constants.h"
#include "DroneMotor.h"

String QUAT_FILE_NAME{"q"};
String FSR_FILE_NAME{"f"};
const int runTimeSeconds{300};
unsigned long startTime{};

/* MOTOR STUFF */
int stepTime{2000};
unsigned long stepStartTime{0};
bool firstTime{1};

DroneMotor motorR(pin::MOTOR_R, pin::POT_R);  // create servo object to control a servo
DroneMotor motorL(pin::MOTOR_L, pin::POT_L);  // create servo object to control a servo
/* END MOTOR STUFF */

// for preference-based learning constants
namespace pbl {
    // controls how long after heel strike to actuate
    float actuateWaitPercent{ .4 };
}

Axo axo(runTimeSeconds);

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(20);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // // MOTOR SETUP
    // motorR.attach(); // some motors need min/max setting
    // motorL.attach(); // some motors need min/max setting
    // delay(1000);
    // motorR.writeMicroseconds(1500);
    // motorL.writeMicroseconds(1500);
    // delay(2000);

    axo.begin();
    // startup condition: wait for high boot tap.
    while (true && !Serial) {
        if (axo.propIMUAvail()) {
            axo.updatePropIMU();
            if (axo.started())
                // exit!
                break;
        }
        delay(10);
    }

    Message m = axo.beginFlash(QUAT_FILE_NAME, FSR_FILE_NAME);
    switch (m) {
        case Message::OK:
            Serial.println("Axo started successfully!");
            break;
        case Message::NO_FLASH_CHIP:
            Serial.println("Flash chip not detected!");
            while (1) { blink(LED_BUILTIN); }
            break;
        case Message::NO_FLASH_SPACE:
            Serial.println("No flash space left! Please erase chip.");
            while (1) { blink(LED_BUILTIN); }
            break;
        case Message::FILE_TOO_LONG:
            Serial.println("File name is too long. Check constants.h for max length.");
            while (1) { blink(LED_BUILTIN); }
            break;
    }

    // PRINTING
    digitalWrite(LED_BUILTIN, LOW);
    Serial.printf("Saving quaternions to: %s, FSR to: %s\n", axo.getQuatSavefile(), axo.getFSRSavefile());
    Serial.println("-----");
    startTime = micros();
    stepStartTime = millis();
}

unsigned long lastTime{};
unsigned long lastIMUSaveTime{};
int stepCount{0};

void loop() {
    if (axo.propUpdated()) {
        if (axo.adaIMUAvail()) {
            // pretty consistently get ~10250 us per sample!
            // Serial.println(micros() - lastTime);
            lastTime = micros();
            axo.updateAdaIMU();
        }
    } else {
        if (axo.propIMUAvail()) {
            axo.updatePropIMU();
            // axo.printData();
            unsigned long currentTime{micros()};
            if (currentTime - lastTime > 10000) {
                // it's like there's an optimal delay here - not sure why
                // or how to get it, but this combo works.

                delayMicroseconds(300);
            }
            // wait 5 seconds to start saving data
            if (currentTime - startTime > 5000000) {
                if (!axo.saveData(currentTime - lastIMUSaveTime)) {
                    Serial.println("File space exceeded.");
                    pinMode(LED_BUILTIN, OUTPUT);
                    while (1) {
                        blink(LED_BUILTIN);
                        motorR.stop();
                        motorL.stop();
                    }
                }
                // make sure the FSR zero time and the quaternion zero time match up.
                if (firstTime) {
                    firstTime = 0;
                    axo.saveFSR(0);
                    stepStartTime = 0;
                }
                lastIMUSaveTime = micros();
                // Serial.println("saving...");
                // axo.printData();
                // axo.printRelQuat();

                // check if we've taken a step (this detects toe lift)
                if (axo.FSRStepped()) {
                    unsigned long currentMSTime{millis()};
                    if (currentMSTime - stepStartTime > 800) {
                        axo.saveFSR(currentMSTime - stepStartTime);
                        stepTime = currentMSTime - stepStartTime;
                        Serial.printf("STEP %d: %d milliseconds\n", stepCount, (currentMSTime - stepStartTime));
                        stepCount++;
                        stepStartTime = currentMSTime;
                    }
                }
            }
        }
    }



    // MOTOR WRITING
    // if (!firstTime) {
    //     unsigned long currentTime = millis();
    //
    //     // Trigger the motors for a brief period
    //     if ((currentTime - stepStartTime > pbl::actuateWaitPercent*stepTime) &&
    //         (currentTime - stepStartTime < (0.2 + pbl::actuateWaitPercent)*stepTime)) {
    //         // write to the motors
    //         motorR.write(MIDDLE - maxSpeed1, currentTime);
    //         motorL.write(MIDDLE + maxSpeed1, currentTime);
    //
    //     // reset the step start time if step time is passed
    //     } else if (currentTime - stepStartTime > stepTime*1.5) {
    //         // reset step time
    //         stepStartTime = currentTime;
    //
    //     } else if (currentTime - stepStartTime > (0.2 + pbl::actuateWaitPercent)*stepTime) {
    //
    //         // trigger a brief backwards pulse of the motors
    //         if (currentTime - stepStartTime < min((0.3 + pbl::actuateWaitPercent)*stepTime, (0.2 + pbl::actuateWaitPercent)*stepTime + 150)) {
    //             // Short backwards movement
    //             motorR.write(MIDDLE + minSpeed1, currentTime);
    //             motorL.write(MIDDLE - minSpeed1, currentTime);
    //         } else {
    //             motorR.stop();
    //             motorL.stop();
    //         }
    //
    //     // otherwise, stop the motors
    //     } else {
    //         motorR.stop(MIDDLE);
    //         motorL.stop(MIDDLE);
    //     }
    // }


    if (Serial.available()) {
        pbl::actuateWaitPercent = Serial.parseInt();
        if (pbl::actuateWaitPercent > 1) {
            pbl::actuateWaitPercent = 1;
        }
        if (pbl::actuateWaitPercent < 0) {
            pbl::actuateWaitPercent = 0;
        }
        printf("Set wait percent to %f'\n", pbl::actuateWaitPercent);
        Serial.read();
    }
}

void blink(int pin) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
}
