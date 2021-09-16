/*
Tests operation of ankle exo using state-based control, informed by IMU angle
readings at leg and foot.

Lorenzo Shaikewitz, 9/15/2021
*/
// Reading IMU
#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

// Reading/writing flash
#include <SerialFlash.h>
#include <SPI.h>

#include "Axo.h"
#include "constants.h"


// Variables for user
String FILE_NAME{"walk"};
const int runTimeSeconds{60};

float startPermil{500};
float endPermil{800};

// timekeeping
unsigned long startTime{};


Axo axo(runTimeSeconds);


void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // wait for button press (or serial opening)
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    while ((digitalRead(BUTTON_PIN) == HIGH) && !Serial) {
        delay(50);
    }

    // connect button as interrupt for abort operation
    attachInterrupt(digitalPinToInterrupt(pin::JOY_BUTTON), abort, RISING);

    Message m = axo.begin(FILE_NAME);
    switch (m) {
        case Message::OK:
            Serial.println("Axo started successfully!");
            break;
        case Message::NO_FLASH_CHIP:
            Serial.println("Flash chip not detected!");
            while (1);
            break;
        case Message::NO_FLASH_SPACE:
            Serial.println("No flash space left! Please erase chip.");
            while (1);
            break;
        case Message::FILE_TOO_LONG:
            Serial.println("File name is too long. Check constants.h for max length.");
            while (1);
            break;
    }

    Serial.print("Saving data to: ");
    Serial.println(axo.getSavefile());
    Serial.println("-----");


    startTime = micros();
}


void loop() {
    // generate a quat
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
                // check time since start just to be sure we are always saving data
                if (currentTime - startTime > 5000000) {
                    if (!axo.saveData()) {
                        Serial.println("File space exceeded.");
                        digitalWrite(LED_BUILTIN, HIGH);
                        while (1) {
                            blink(LED_BUILTIN);
                        }
                    }
                }
            } else {
                if (!axo.saveData()) {
                    Serial.println("File space exceeded.");
                    digitalWrite(LED_BUILTIN, HIGH);
                    while (1) {
                        blink(LED_BUILTIN);
                    }
                }
                // Serial.println("saving...");
                // axo.printData();
                // axo.printRelQuat();
            }


            // work with the relative quaternion
            axo.updateState();

        }
    }
}


void blink(int pin) {
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
    delay(1000);
}

void abort() {
    axo.abort();
}
