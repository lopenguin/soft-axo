/*
Sensor and motor carrier class for ankle exoskeleton. Designed to abstract
essential but lower-level functions away from Powered.ino.

Lorenzo Shaikewitz, 4/19/2022
*/
#ifndef AXO_CLASS_H
#define AXO_CLASS_H

#include <Arduino.h>
#include <Metro.h>  // easy timers
#include <IntervalTimer.h>  // precise interrupt timer

#include "constants.h"

class Axo {
public:
    Axo() : m_timerIMU{timers::IMU},
            m_timerFSR{timers::FSR},
            m_timerLoad{timers::LOAD},
            m_timerMotor{}
    {/*does nothing*/}


    /* MOTOR FUNCTIONS */
    // starts up motors and potentiometer interrupt routine
    bool beginMotors();
    // Ends motor interrupts (use when changing desired trajectory)
    bool stopMotors();


    /* SENSOR FUNCTIONS */
    // starts up IMU, FSR, load cell, AND motor pots.
    // Freezes if any sensor fails to initialize. Call once in setup().
    bool beginSensors();
    // reads IMU, FSR, and load cell (NO motor pots). Uses metro timers, so
    // call repeatedly in loop (no delays needed)
    bool readSensors();

private:
    // Metro timers (for sensors)
    Metro m_timerIMU;
    Metro m_timerFSR;
    Metro m_timerLoad;

    // Interrupt timer (for motor)
    IntervalTimer m_timerMotor;
};

#endif
