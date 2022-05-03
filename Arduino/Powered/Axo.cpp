/*
Implements Axo class

Lorenzo Shaikewitz, 4/25/2022
*/
#include "Axo.h"
#include "constants.h"
#include <Arduino.h>
#include <IntervalTimer.h>  // for servos

// IMU
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


/* MOTOR FUNCTIONS */
static IntervalTimer motorTimer;
static volatile bool useISR{};
static volatile int prevPotL{};
static volatile int prevPotR{};
static unsigned int targetAngle{};

void Axo::beginMotors() {
    // start up motor servo branches
    m_leftMotor.attach(pin::ESC_L);
    m_rightMotor.attach(pin::ESC_R);

    delay(1000);

    m_leftMotor.writeMicroseconds(motor::CAL_PWM);
    m_rightMotor.writeMicroseconds(motor::CAL_PWM);

    delay(motor::CAL_DELAY);

    // start up the motor timer (with low priority)
    motorTimer.begin(motorISR, timer::MOTOR_US);
    motorTimer.priority(0xFF);
    useISR = false;
}

void Axo::setAngle(int val) {
    // turn on the interrupt
    useISR = true;
    targetAngle = val;
}

void Axo::stopMotors() {
    // turn off interrupt
    useISR = false;
    m_leftMotor.writeMicroseconds(motor::CENTER);
    m_rightMotor.writeMicroseconds(motor::CENTER);
}

void Axo::detachMotors() {
    // stop motors
    stopMotors();
    // end the motor interrupt timer
    motorTimer.end();
    // detach the servos
    m_leftMotor.detach();
    m_rightMotor.detach();
}


void motorISR(void) {
    if (!useISR)
        return;
    
    // read potentiometers
    int currPotL = digitalRead(pin::POT_L);
    int currPotR = digitalRead(pin::POT_R);

    // check for a full rotation
    // LEFT
    if ((currPotL - prevPotL < -motor::NOISE_JUMP))
        ++turnsL;
    else if ((currPotL - prevPotL > motor::NOISE_JUMP))
        --turnsL;
    prevPotL = currPotL;
    // RIGHT
    if ((currPotR - prevPotR < -motor::NOISE_JUMP))
        ++turnsR;
    else if ((currPotR - prevPotR > motor::NOISE_JUMP))
        --turnsR;
    prevPotR = currPotR;

    // use p control to compute target
    lMS = p(targetAngle, currPotL + 1023*turnsL);
    rMS = p(targetAngle, currPotR + 1023*turnsR);

    // check if we are sufficiently close to goal (TODO: TEST!!! dangerous)
    // if ((abs(lMS - motor::CENTER) < motor::LOWMS_THRESHOLD) && (abs(rMS - motor::CENTER) < motor::LOWMS_THRESHOLD) {
    //     // stop motors and turn off interrupt
    //     lMS = motor::CENTER;
    //     rMS = motor::CENTER;
    //     useISR = false;
    // }
    
    m_leftMotor.writeMicroseconds(lMS);
    m_rightMotor.witeMicroseconds(rMS);
}

int p(int desired, int curr) {
    curr = max(0, min(1023, curr)   // make sure current position is bounded

    int ret = 1500 + (desired - curr) * motor::K_P);
    ret += pow(motor::E,1 / (motor::C * curr));
    ret -= pow(motor::E, 1 / (motor::C * (motor::POT_MAX - curr)));
    return min(motor::MAX_FORWARD, max(motor::MAX_BACKWARD, ret));
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

    // set IMU offsets
    m_shinIMU.setSensorOffsets(offsets::shin);
    m_footIMU.setSensorOffsets(offsets::foot);

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
