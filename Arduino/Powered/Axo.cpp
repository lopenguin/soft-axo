/*
Implements Axo class

Lorenzo Shaikewitz, 4/25/2022
*/
#include "Axo.h"
#include "constants.h"
#include <Arduino.h>

// IMU
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


/* MOTOR FUNCTIONS */
bool Axo::beginMotors() {
    // start up motors


    m_timerMotor.begin(motorInterrupt, timer::MOTOR_US);
}


/* SENSOR FUNCTIONS */
void Axo::begin() {
    /** General Begins **/
    pinMode(pin::LEDB, OUTPUT);
    pinMode(pin::LEDG, OUTPUT);
    SerialOut.begin(BAUD);
    // TODO: begin e-stops


    /** IMUs **/
    bool shinIMUStarted = m_shinIMU.begin();
    bool footIMUStarted = m_footIMU.begin();

    if (!shinIMUStarted) {
        SerialOut.println("Error connecting to shin IMU. Foot IMU status unknown.");
        while (1);
    }
    if (!footIMUStarted) {
        SerialOut.println("Error connecting to foot IMU. Shin IMU successfully connected.");
        while (1);
    }

    delay(1000);

    // set up external crystal for optimal performance
    m_shinIMU.setExtCrystalUse(true);
    m_footIMU.setExtCrystalUse(true);

    // Switch to NDOF mode--use all 9 DOFs, 100 Hz fusion data
    m_shinIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
    m_footIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);


    /** FSR **/
    pinMode(pin::FSR, INPUT);


    /** Load Cell **/
    pinMode(pin::LOADCELL, INPUT);
}

void readSensors() {
    if (m_timerIMU.check()) {
        /* DATA OUTPUT FORMAT:
        TODO
        */
        
    }

    if (m_timerFSR.check()) {

    }

    if (m_timerLoad.check()) {

    }
}
