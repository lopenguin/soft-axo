/*
Main code to run the ankle exoskeleton. Sensor gathering/printing, low-level
motor control, and other secondary functions are abstracted into the Axo class.
This file focuses on the high-level control scheme.

CONTROL: FSR PUSHOFF DETECTION

Lorenzo Shaikewitz, 4/19/2022
*/
#include "Axo.h"
#include "constants.h"
#include "curves.h"
#include <Metro.h>

Axo axo;

Metro controlTimer{20}; // 50 Hz
unsigned long startTime{};
unsigned long lastStartTime{};
unsigned long stepTime{2000};
unsigned long lastStepTime{2000};
bool fsrLow{};

void setup() {
    // start up sensors
    axo.begin();
    SerialOut.println("MODE: FSR PUSHOFF DETECTION");
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
    lastStartTime = millis();
    startTime = millis();
}


void loop() {
    // angle control updating
    if (controlTimer.check()) {
        // compute the time elapsed
        unsigned long currentTime = millis();
        unsigned long dt = currentTime - lastStartTime;

        // execute control
        if (fsrPushoff(axo, dt, lastStepTime)) {
            lastStepTime = stepTime;
            lastStartTime = startTime;
        }

        // check the FSR for a step
        int fsr = axo.getFSR();
        if (fsr < control::FSR_THRESH) {
            // want to record only the first heel strike
            if (!fsrLow) {
                fsrLow = true;
                stepTime = dt;
                startTime = currentTime;
                #ifndef SUPPRESS_LOG
                unsigned long t{millis() - axo.startTime()};
                SerialOut.printf("\nLOG,%d | Push-off detected %u", t, dt);
                #endif
            }
        } else {
            // SerialOut.printf("\nLOG,%d | Push-off detected.", millis() - axo.startTime());
            fsrLow = false;
        }
    }

    axo.spin();

    if (SerialOut.available()) {
        char c = SerialOut.read();

        switch (c) {
            case 'x': {
                // stop!
                #ifndef SUPPRESS_LOG
                unsigned long t{millis() - axo.startTime()};
                SerialOut.printf("\nLOG,%d | Received x. Terminating Axo...",t);
                #endif
                axo.detachMotors();
                axo.setBlueLED(HIGH);
                axo.setGreenLED(LOW);

                // pause the loop
                while (1);
                break;
            }

            default: {
                #ifndef SUPPRESS_LOG
                unsigned long t{millis() - axo.startTime()};
                SerialOut.printf("\nLOG,%d | Received %c. No action taken.",t,c);
                #endif
                break;
            }
        }
    }
}