/*
Implements Axo class

Lorenzo Shaikewitz, 4/25/2022
*/
#include "Axo.h"
#include "constants.h"


/* MOTOR FUNCTIONS */
bool Axo::beginMotors() {
    // start up motors


    m_timerMotor.begin(motorInterrupt, timer::MOTOR_US);
}
