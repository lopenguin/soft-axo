/*
Main code to run the ankle exoskeleton. Sensor gathering/printing, low-level
motor control, and other secondary functions are abstracted into the Axo class.
This file focuses on the high-level control scheme.

Lorenzo Shaikewitz, 4/19/2022
*/
#include "Axo.h"
#include "constants.h"

Axo axo;

void setup() {
    // start up sensors
    axo.begin();
    axo.setGreenLED(HIGH);

    // wait for startup command
    while (1) {
        if (SerialOut.available()) {
            
        }
        delay(100);
    }

}


void loop() {

}
