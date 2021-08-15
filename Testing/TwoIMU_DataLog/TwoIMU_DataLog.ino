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
const char FILE_NAME[10]{"testMe"};
const int runTimeSeconds{300};

Axo axo(runTimeSeconds);

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    while (!Serial) {
        delay(50);
    }

    Message m = axo.begin();
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
    }

    Serial.print("Saving data to: ");
    Serial.println(axo.getSavefile());
    Serial.println("-----");
}

void loop() {
    if (axo.propIMUAvail()) {
        axo.updateIMUs();
        axo.printData();
    }
}
