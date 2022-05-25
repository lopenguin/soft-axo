/*
Contains functions to prescribe a curve for the Axo

Lorenzo Shaikewitz, 5/5/2022 (Cinco de Mayo!)
*/
#ifndef AXO_CURVES_H
#define AXO_CURVES_H

#include "Axo.h"
#include "constants.h"

/* Bang-bang control at pushoff
Duplicate of version used in Febuary.
STATE VARIABLE: time since last heel strike (trigged by FSR)

Idea: at 40% of gait time trigger motors for 10% of gait time
*/
// constants
static constexpr float triggerPercent{0.37};
static constexpr int maxAngle{800};
static constexpr float motorOnPercent{0.3};
static constexpr int zeroAngle{0};

void bangBangAtPushoff(Axo& axo, unsigned long timeSinceLastHeelStrike, unsigned long lastStepTime) {

    if ((timeSinceLastHeelStrike > triggerPercent*lastStepTime) &&
        (timeSinceLastHeelStrike < ((motorOnPercent + triggerPercent)*lastStepTime))) {
        // write the maximum safe angle
        axo.setAngle(maxAngle);
    }
    else if (timeSinceLastHeelStrike > ((motorOnPercent + triggerPercent)*lastStepTime)) {
        // trigger a brief backwards pulse
        axo.setAngle(zeroAngle);
    } 
    else {
        axo.stopMotors();
    }

    if (timeSinceLastHeelStrike > ((2*motorOnPercent + triggerPercent)*lastStepTime)) {
        axo.stopMotors();
    }
}
//                        ---Associated axo control code---
// unsigned long startTime{}; // Initialize to millis() in setup
// unsigned long stepTime{2000};
// bool fsrHigh{};
// -------------------------------------------------------------
// loop():
//     if (controlTimer.check()) {
//         // compute the time elapsed
//         unsigned long currentTime = millis();
//         unsigned long dt = currentTime - startTime;

//         // execute control
//         bangBangAtPushoff(axo, dt, stepTime);

//         // check the FSR for a step
//         int fsr = axo.getFSR();
//         if (fsr > control::FSR_THRESH) {
//             // want to record only the first heel strike
//             if (!fsrHigh) {
//                 fsrHigh = true;
//                 stepTime = dt;
//                 startTime = currentTime;
//                 #ifndef SUPPRESS_LOG
//                 unsigned long t{millis() - axo.startTime()};
//                 SerialOut.printf("\nLOG,%d | Step Recorded! %u", t, dt);
//                 #endif
//             }
//         } else {
//             fsrHigh = false;
//         }
//     }



/* FSR release-

*/

#endif