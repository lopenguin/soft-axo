/*
Testing data logging of dual IMU data using Axo carrier class

That means:
- IMU reading and flash saving is confined to the Axo and IMUCarrier classes.
- Both IMUs should output data
- this is experimental code.

Lorenzo Shaikewitz, 8/14/2021 TEST!!!!!
*/

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

#include "Axo.h"
#include "constants.h"

const short BUTTON_PIN{23};
String FILE_NAME{"testMe"};
const int runTimeSeconds{60};

Axo axo(runTimeSeconds);

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    while (!Serial) {
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
}

unsigned long startTime{};

// consider adding 10 second delay to savedata so save has time to make constant.
void loop() {
    if (axo.propUpdated()) {
        if (axo.adaIMUAvail()) {
            Serial.println(micros() - startTime);
            startTime = micros();
            axo.updateAdaIMU();
        }
    } else {
        if (axo.propIMUAvail()) {
            axo.updatePropIMU();
            axo.printData();

            if (!axo.saveData()) {
                Serial.println("File space exceeded.");
                digitalWrite(LED_BUILTIN, HIGH);
                while (1) {
                    blink(LED_BUILTIN);
                }
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
