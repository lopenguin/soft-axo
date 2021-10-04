/*
Testing data logging of dual IMU data using Axo carrier class

That means:
- IMU reading and flash saving is confined to the Axo and IMUCarrier classes.
- Both IMUs should output data
- this is experimental code.

Lorenzo Shaikewitz, 8/14/2021
*/

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

#include "Axo.h"
#include "constants.h"

const short BUTTON_PIN{21};
String FILE_NAME{"testMe"};
const int runTimeSeconds{60};
unsigned long startTime{};

Axo axo(runTimeSeconds);

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    while ((digitalRead(BUTTON_PIN) == HIGH) && !Serial) {
        delay(50);
    }


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

unsigned long lastTime{};

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
        }
    }
}

void blink(int pin) {
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
    delay(1000);
}
