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
    // start up motors (TODO)


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

    // extra delay just to be safe
    delay(1000);
}

void Axo::readSensors() {
    if (m_timerIMU.check()) {
        /* DATA OUTPUT FORMAT:
        TODO
        */
        // Calibration status
        m_shinIMU.getCalibration(&m_quatCalShin[0], &m_quatCalShin[1], &m_quatCalShin[2], &m_quatCalShin[3]);
        m_footIMU.getCalibration(&m_quatCalFoot[0], &m_quatCalFoot[1], &m_quatCalFoot[2], &m_quatCalFoot[3]);

        // Quaternions
        m_quatShin = m_shinIMU.getQuat();
        m_quatFoot = m_footIMU.getQuat();

        // For printing: acceleration, etc?
        sensors_event_t shinAccel{};
        m_shinIMU.getEvent(&shinAccel, Adafruit_BNO055::VECTOR_ACCELEROMETER);
        sensors_event_t footAccel{};
        m_footIMU.getEvent(&footAccel, Adafruit_BNO055::VECTOR_ACCELEROMETER);

        printIMUs(&shinAccel, &footAccel);
    }

    if (m_timerFSR.check()) {
        m_fsrVal = analogRead(pin::FSR);
        SerialOut.printf("FSR,%u", m_fsrVal);
    }

    if (m_timerLoad.check()) {
        m_loadCellVal = analogRead(pin::LOADCELL);
        SerialOut.printf("Load,%u", m_loadCellVal);
    }
}



/* PRINT FUNCTIONS */
void Axo::printIMUs(sensors_event_t* shinAccel, sensors_event_t* footAccel) {
    // FORMAT:
    // IMU, shin calibration (total, gyro, accel, mag), foot calibration,
    //      shin quaternion (qw, qx, qy, qz), foot quaternion
    //      shin accel (ax, ay, az), foot accel

    SerialOut.printf("IMU,(%u, %u, %u, %u),(%u, %u, %u, %u),"
                    "(%.4f, %.4f, %.4f, %.4f),(%.4f, %.4f, %.4f, %.4f),"
                    "(%.4f, %.4f, %.4f),(%.4f, %.4f, %.4f)",
        m_quatCalShin[0], m_quatCalShin[1], m_quatCalShin[2], m_quatCalShin[3],
        m_quatCalFoot[0], m_quatCalFoot[1], m_quatCalFoot[2], m_quatCalFoot[3],
        m_quatShin.w(), m_quatShin.x(), m_quatShin.y(), m_quatShin.z(),
        m_quatFoot.w(), m_quatFoot.x(), m_quatFoot.y(), m_quatFoot.z(),
        shinAccel->acceleration.x, shinAccel->acceleration.y, shinAccel->acceleration.z,
        footAccel->acceleration.x, footAccel->acceleration.y, footAccel->acceleration.z);
}
