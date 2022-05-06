/*
Main code to run the ankle exoskeleton. Sensor gathering/printing, low-level
motor control, and other secondary functions are abstracted into the Axo class.
This file focuses on the high-level control scheme.

Lorenzo Shaikewitz, 4/19/2022
*/
#include "Axo.h"
#include "constants.h"
#include "curves.h"
#include <Metro.h>

Axo axo;

Metro controlTimer{20}; // 50 Hz
int angle{500};

void setup() {
    // start up sensors
    axo.begin();
    axo.setBlueLED(HIGH);

    // wait for startup command
    SerialOut.println("Waiting for startup command...");
    while (1) {
        if (SerialOut.available()) {
            if (SerialOut.read() == 's') {
                SerialOut.println("Startup received!");
                break;
            }
        }
        delay(100);
    }
    axo.printKey();
    axo.setBlueLED(LOW);
    axo.setGreenLED(HIGH);

    // start motors
    axo.beginMotors();

    // reset control timer
    controlTimer.reset();
}


void loop() {
    // angle control updating
    if (controlTimer.check()) {
        unsigned long currentTime = millis();
        unsigned long timeDiff = currentTime - axo.getStepStartTime();

        // execute control
        bangBangAtPushoff(axo, timeDiff, axo.getStepStartTime());
    }

    axo.spin();

    if (SerialOut.available()) {
        char c = SerialOut.read();

        switch (c) {
            case 'x':
                // stop!
                #ifndef SUPPRESS_LOG
                SerialOut.println("LOG,Received x. Terminating Axo...");
                #endif
                axo.detachMotors();
                axo.setBlueLED(HIGH);
                axo.setGreenLED(LOW);

                // pause the loop
                while (1);
                break;

            case 'a': {
                // expect a 4-digit angle command
                angle = SerialOut.parseInt();
                if (angle > motor::POT_MAX) angle = motor::POT_MAX;
                if (angle < motor::POT_MIN) angle = motor::POT_MIN;
                #ifndef SUPPRESS_LOG
                SerialOut.printf("LOG,Received a. Setting angle to %d\n", angle);
                #endif
                break;
            }

            case 's': {
                #ifndef SUPPRESS_LOG
                axo.stopMotors();
                SerialOut.printf("LOG,Received s. Stopping motor...\n");
                #endif
            }

            default:
                #ifndef SUPPRESS_LOG
                SerialOut.printf("LOG,Received %c. No action taken.\n",c);
                #endif
                break;
        }
    }
}