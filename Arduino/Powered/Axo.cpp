/*
Implements Axo class

Lorenzo Shaikewitz, 4/25/2022
*/
#include "Axo.h"
#include "constants.h"
#include <Arduino.h>
#include <IntervalTimer.h>  // for servos
#include <Servo.h>

// IMU
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


/* MOTOR FUNCTIONS */
int p(int desired, int curr);

void Axo::beginMotors() {
    // start up motor servo branches
    m_leftMotor.attach(pin::ESC_L);
    m_rightMotor.attach(pin::ESC_R);

    delay(1000);

    m_leftMotor.writeMicroseconds(motor::CAL_PWM);
    m_rightMotor.writeMicroseconds(motor::CAL_PWM);

    delay(motor::CAL_DELAY);

    // make sure the motor starts off
    m_useMotorMetro = false;

    // reset the turns counters
    m_prevPotL = analogRead(pin::POT_L);
    m_prevPotR = analogRead(pin::POT_R);
    m_turnsL = 0;
    m_turnsR = 0;

    #ifndef SUPPRESS_LOG
    unsigned long t{millis() - m_startTime};
    SerialOut.printf("LOG,%s | Motors started.\n",int_to_char(t));
    #endif

    // reset the metro timers
    m_timerIMU.reset();
    m_timerFSR.reset();
    m_timerLoad.reset();
    m_timerHeartbeat.reset();
}

void Axo::setAngle(int val) {
    // turn on the interrupt
    m_useMotorMetro = true;
    m_targetAngleL = motor::LZERO - val;
    m_targetAngleR = motor::RZERO + val;
}

void Axo::stopMotors() {
    // turn off interrupt
    m_useMotorMetro = false;
    m_leftMotor.writeMicroseconds(motor::CENTER);
    m_rightMotor.writeMicroseconds(motor::CENTER);
}

void Axo::detachMotors() {
    // stop motors
    stopMotors();
    // detach the servos
    m_leftMotor.detach();
    m_rightMotor.detach();
}


void Axo::updateMotors() {
    if (!m_useMotorMetro)
        return;
    
    // read potentiometers
    int currPotL = analogRead(pin::POT_L);
    int currPotR = analogRead(pin::POT_R);

    // check for a full rotation
    // LEFT
    if ((currPotL - m_prevPotL < -motor::NOISE_JUMP))
        ++m_turnsL;
    else if ((currPotL - m_prevPotL > motor::NOISE_JUMP))
        --m_turnsL;
    m_prevPotL = currPotL;
    // RIGHT
    if ((currPotR - m_prevPotR < -motor::NOISE_JUMP))
        ++m_turnsR;
    else if ((currPotR - m_prevPotR > motor::NOISE_JUMP))
        --m_turnsR;
    m_prevPotR = currPotR;

    // use p control to compute target
    int lMS{ p(m_targetAngleL, currPotL + 1023*m_turnsL) };
    int rMS{ p(m_targetAngleR, currPotR + 1023*m_turnsR) };

    // check if we are sufficiently close to goal (TODO: TEST!!! dangerous)
    // if ((abs(lMS - motor::CENTER) < motor::LOWMS_THRESHOLD) && (abs(rMS - motor::CENTER) < motor::LOWMS_THRESHOLD) {
    //     // stop motors and turn off interrupt
    //     lMS = motor::CENTER;
    //     rMS = motor::CENTER;
    //     m_useMotorMetro = false;
    // }
    
    Axo::m_leftMotor.writeMicroseconds(lMS);
    Axo::m_rightMotor.writeMicroseconds(rMS);

    #ifndef SUPPRESS_MOTOR
    unsigned long t{millis() - m_startTime};
    SerialOut.printf("M %s%s%s%s%s%s%s%s%s\n", 
                    int_to_char(t), int_to_char(currPotL), int_to_char(currPotR),
                    int_to_char(currPotL + 1023*m_turnsL), int_to_char(currPotR + 1023*m_turnsR),
                    int_to_char(m_targetAngleL), int_to_char(m_targetAngleR),
                    int_to_char(lMS), int_to_char(rMS));
    #endif
}

int p(int desired, int curr) {
    curr = max(motor::POT_MIN, min(motor::POT_MAX, curr));   // make sure current position is bounded

    int ret = motor::CENTER + (desired - curr) * motor::K_P;
    ret += pow(motor::E,1 / (motor::C * curr));
    ret -= pow(motor::E, 1 / (motor::C * (motor::POT_MAX - curr)));
    return min(motor::MAX_FORWARD, max(motor::MAX_BACKWARD, ret));
}


/* SENSOR FUNCTIONS */
void Axo::begin(bool useIMUOffsets) {
    /** General Begins **/
    pinMode(pin::LEDB, OUTPUT);
    pinMode(pin::LEDG, OUTPUT);
    SerialOut.begin(BAUD);
    SerialOut.setTimeout(timer::SERIAL_TIMEOUT);
    m_startTime = millis();
    // TODO: begin e-stops

    SerialOut.println("Axo started.");
    SerialOut.println("------------------------------------------------------------------");
    SerialOut.println("Commands:");
    SerialOut.println("* Startup command: 's'");
    SerialOut.println("* Stop command: 'x'");
    SerialOut.println("All messages will be in CSV format once start command is received.");
    SerialOut.println("* Format: FLAG,data1,data2,...");
    SerialOut.println("* Active flags: ERR, LOG, IMU, FSR, LOAD, MOTOR");
    SerialOut.println("First print will give key in text format.");
    SerialOut.println("------------------------------------------------------------------");


    /** IMUs **/
    bool shinIMUStarted = m_shinIMU.begin();
    bool footIMUStarted = m_footIMU.begin();

    if (!shinIMUStarted) {
        SerialOut.println("ERR,Error connecting to shin IMU. Foot IMU status unknown.");
        while (1);
    }
    if (!footIMUStarted) {
        SerialOut.println("ERR,Error connecting to foot IMU. Shin IMU successfully connected.");
        while (1);
    }

    delay(1000);

    // set up external crystal for optimal performance
    m_shinIMU.setExtCrystalUse(true);
    m_footIMU.setExtCrystalUse(true);

    // set IMU offsets
    if (useIMUOffsets) {
        m_shinIMU.setSensorOffsets(offsets::shin);
        m_footIMU.setSensorOffsets(offsets::foot);
    }

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

void Axo::spin() {
    // call the heartbeat (no heartbeat needed)
    // heartbeat();

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

        #ifndef SUPPRESS_IMU
        printIMUs(&shinAccel, &footAccel);
        #endif
    }

    if (m_timerFSR.check()) {
        m_fsrVal = analogRead(pin::FSR);
        #ifndef SUPPRESS_FSR
        unsigned long t{millis() - m_startTime};
        SerialOut.printf("F %s%s\n", int_to_char(t), int_to_char(m_fsrVal));
        #endif
    }

    if (m_timerLoad.check()) {
        m_loadCellVal = analogRead(pin::LOADCELL);
        #ifndef SUPPRESS_LOAD
        unsigned long t{millis() - m_startTime};
        SerialOut.printf("L %s%s\n", int_to_char(t), int_to_char(m_loadCellVal));
        #endif
    }

    if (m_timerMotor.check()) {
        updateMotors();
    }
}


void Axo::heartbeat() {
    if (m_timerHeartbeat.check()) {
        #ifndef SUPPRESS_LOG
        unsigned long t{millis() - m_startTime};
        SerialOut.printf("LOG,%s | Heartbeat\n",int_to_char(t));
        #endif
    }
}


/* PRINT FUNCTIONS */
void Axo::printIMUs(sensors_event_t* shinAccel, sensors_event_t* footAccel) {
    // FORMAT:
    // IMU, shin calibration (total, gyro, accel, mag), foot calibration,
    //      shin quaternion (qw, qx, qy, qz), foot quaternion
    //      shin accel (ax, ay, az), foot accel

    // convert floats to integers
    uint16_t quatShin_w = floatToInt(m_quatShin.w());
    uint16_t quatShin_x = floatToInt(m_quatShin.x());
    uint16_t quatShin_y = floatToInt(m_quatShin.y());
    uint16_t quatShin_z = floatToInt(m_quatShin.z());

    uint16_t quatFoot_w = floatToInt(m_quatFoot.w());
    uint16_t quatFoot_x = floatToInt(m_quatFoot.x());
    uint16_t quatFoot_y = floatToInt(m_quatFoot.y());
    uint16_t quatFoot_z = floatToInt(m_quatFoot.z());

    uint16_t shinAccel_x = floatToInt(shinAccel->acceleration.x,2);
    uint16_t shinAccel_y = floatToInt(shinAccel->acceleration.y,2);
    uint16_t shinAccel_z = floatToInt(shinAccel->acceleration.z,2);
    
    uint16_t footAccel_x = floatToInt(footAccel->acceleration.x,2);
    uint16_t footAccel_y = floatToInt(footAccel->acceleration.y,2);
    uint16_t footAccel_z = floatToInt(footAccel->acceleration.z,2);

    SerialOut.printf("I %s%s%s%s%s%s%s%s"
                    "%s%s%s%s%s%s%s%s"
                    "%s%s%s%s%s%s\n",
        int_to_char(m_quatCalShin[0]), int_to_char(m_quatCalShin[1]), int_to_char(m_quatCalShin[2]), int_to_char(m_quatCalShin[3]),
        int_to_char(m_quatCalFoot[0]), int_to_char(m_quatCalFoot[1]), int_to_char(m_quatCalFoot[2]), int_to_char(m_quatCalFoot[3]),
        int_to_char(quatShin_w), int_to_char(quatShin_x), int_to_char(quatShin_y), int_to_char(quatShin_z),
        int_to_char(quatFoot_w), int_to_char(quatFoot_x), int_to_char(quatFoot_y), int_to_char(quatFoot_z),
        int_to_char(shinAccel_x), int_to_char(shinAccel_y), int_to_char(shinAccel_z),
        int_to_char(footAccel_x), int_to_char(footAccel_y), int_to_char(footAccel_z));
}


void Axo::printKey() {
    SerialOut.println("IMU,shin_cal_tot,shin_cal_gyr,shin_cal_acc,shin_cal_mag | foot_cal_tot,foot_cal_gyr,foot_cal_acc,foot_cal_mag | "
                      "shin_qw,shin_qx,shin_qy,shin_qz | foot_qw,foot_qx,foot_qy,foot_qz | shin_ax,shin_ay,shin_az | foot_ax,foot_ay,foot_az");

    SerialOut.println("FSR,time | val");
    SerialOut.println("LOAD,time | val");
    SerialOut.println("MOTOR,time | potL_cur,potR_cur | potL_adj,potR_adj | targetPotL,targetPotR | motorCmdL,motorCmdR");
    SerialOut.println("LOG,time | msg");
    SerialOut.println("ERR,msg");
}

// converts float to 2 characters
uint16_t Axo::floatToInt(float f, int precision) {
    return static_cast<uint16_t>(f * pow(10, precision));
}