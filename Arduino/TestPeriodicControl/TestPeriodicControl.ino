/*
Code for testing periodic control of the ankle exoskeleton.
You must set a constant gait period and a bang-bang percentage.
The exo will follow.

Sensors needed:
- Motor potentiometers
- load cell (optional)

Lorenzo Shaikewitz, 1/8/2023
*/
#include "Axo.h"
#include "constants.h"
// #include "curves.h"
#include <Metro.h>

Axo axo;

Metro controlTimer{100}; // 100 Hz
unsigned long startTime{};
constexpr unsigned long stepTime{1500}; // SET STEP TIME HERE
bool fsrHigh{};
float backPulseTime{500};

float triggerPercent{0.35};
float newTriggerPercent{0.35};
float motorOnPercent{0.35};
float newMotorOnPercent{0.35};

void setup() {
    // start up sensors
    axo.begin();
    SerialOut.println("TESTING PERIODIC CONTROL");
    axo.setBlueLED(HIGH);

    // wait for startup command
    SerialOut.println("Waiting for startup command...");
    while (1) {
        if (SerialOut.available()) {
            if (SerialOut.read() == 's') {
                SerialOut.println("Startup received!");
                triggerPercent = newTriggerPercent;
                motorOnPercent = newMotorOnPercent;
                break;
            }
        }
        else if (axo.resetEstop()) {
            SerialOut.println("Startup received!");
            delay(500);
            axo.resetEstop();
            break;
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
    startTime = millis();
}


void loop() {
    // angle control updating
    if (controlTimer.check()) {
        // compute the time elapsed
        unsigned long currentTime = millis();
        unsigned long dt = currentTime - startTime;

        // execute control
        bangBangAtPushoff(axo, dt, stepTime);

        // keep a constant period
        if (dt > stepTime) {
            startTime = currentTime;

            // update trigger percentages at the end of every step
            triggerPercent = newTriggerPercent;
            motorOnPercent = newMotorOnPercent;
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
                axo.stopMotors();
                delay(1000);
                axo.detachMotors();
                axo.setBlueLED(HIGH);
                axo.setGreenLED(LOW);

                // pause the loop
                while (1);
                break;
            }

            case 'a': {
                // expect a float percentage
                axo.stopMotors();
                newTriggerPercent = SerialOut.parseFloat();
                if (newTriggerPercent > 1.0)
                    newTriggerPercent = 1.0;
                if (newTriggerPercent < 0.0)
                    newTriggerPercent = 0.0;
                #ifndef SUPPRESS_LOG
                SerialOut.printf("LOG,Received a. Setting trigger percent to %f\n", newTriggerPercent);
                #endif
                break;
            }

            case 't': {
                // expect a float percentage
                axo.stopMotors();
                newMotorOnPercent = SerialOut.parseFloat();
                if (newMotorOnPercent > 1.0)
                    newMotorOnPercent = 1.0;
                if (newMotorOnPercent < 0.0)
                    newMotorOnPercent = 0.0;
                #ifndef SUPPRESS_LOG
                SerialOut.printf("LOG,Received a. Setting run time to %d\n", newMotorOnPercent);
                #endif
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



static constexpr int maxAngle{650};
static constexpr int zeroAngle{0};

void bangBangAtPushoff(Axo& axo, unsigned long deltat, unsigned long predStepTime) {
    float a{ triggerPercent*predStepTime };                 // pulse starts
    float b{ a + motorOnPercent*predStepTime };             // pulse ends
    float c{ min(predStepTime, b + backPulseTime)};         // buffer time for backwards pulse
    // SerialOut.printf("dt: %.4f, a: %.4f, b: %.4f, c: %.4f", deltat, a, b, c);

    // Follow a piecewise curve
    // BEFORE *TRIGGERPERCENT*
    if (deltat < a) {
        // initially no motion
        axo.stopMotors();
    } else if ((deltat >= a) && (deltat <= b)) {
        // spike at pushoff
        axo.setAngle(maxAngle);
    } else if ((deltat > b) && (deltat <= c)) {
        // brief backwards pulse
        axo.setAngle(zeroAngle);
    } else if (deltat > c) {
        // return to no motion
        axo.stopMotors();
    }
}

// void bangBangAtPushoff(Axo& axo, unsigned long timeSinceLastHeelStrike, unsigned long lastStepTime) {

//     if ((timeSinceLastHeelStrike > triggerPercent*lastStepTime) &&
//         (timeSinceLastHeelStrike < ((motorOnPercent + triggerPercent)*lastStepTime))) {
//         // write the maximum safe angle
//         axo.setAngle(maxAngle);
//     }
//     else if (timeSinceLastHeelStrike > ((motorOnPercent + triggerPercent)*lastStepTime)) {
//         // trigger a brief backwards pulse
//         axo.setAngle(zeroAngle);
//     } 
//     else {
//         axo.stopMotors();
//     }

//     if (timeSinceLastHeelStrike > ((2*motorOnPercent + triggerPercent)*lastStepTime)) {
//         axo.stopMotors();
//         triggerPercent = newTriggerPercent;
//         motorOnPercent = newMotorOnPercent;
//     }
// }