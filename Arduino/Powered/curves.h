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
static constexpr float triggerPercent{0.4};
static constexpr int maxAngle{600};
static constexpr int zeroAngle{0};

void bangBangAtPushoff(Axo& axo, unsigned long timeSinceLastHeelStrike, unsigned long lastStepTime) {

    if ((timeSinceLastHeelStrike > triggerPercent*lastStepTime) &&
        (timeSinceLastHeelStrike < ((0.2 + triggerPercent)*lastStepTime))) {
        // write the maximum safe angle
        axo.setAngle(maxAngle);
    }
    else if (timeSinceLastHeelStrike > ((0.2 + triggerPercent)*lastStepTime)) {
        // trigger a brief backwards pulse
        axo.setAngle(zeroAngle);
    } 
    else {
        axo.stopMotors();
    }

    if (timeSinceLastHeelStrike > ((0.4 + triggerPercent)*lastStepTime)) {
        axo.stopMotors();
    }
}

#endif