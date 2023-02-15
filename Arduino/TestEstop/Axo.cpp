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

volatile bool Axo::m_estopPressed;

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
    SerialOut.printf("\nLOG,%d | Motors started.",t);
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
    // DEBUG
    // SerialOut.printf("%d, %d\n", lMS, rMS);
    // SerialOut.printf("%d, %d\n", m_targetAngleR, currPotR + 1023*m_turnsR);

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
    // print data
    unsigned long t{millis() - m_startTime};
    SerialOut.print("\nM ");
    writeBytes(t, 4);
    writeBytes(currPotL);   writeBytes(currPotR);
    writeBytes(currPotL + 1023*m_turnsL);   writeBytes(currPotR + 1023*m_turnsR);
    writeBytes(m_targetAngleL);    writeBytes(m_targetAngleR);
    writeBytes(lMS); writeBytes(rMS);
    #endif
}

int p(int desired, int curr) {
    // change curr to be between 0 and 1023
    curr = max(motor::POT_MIN, min(motor::POT_MAX, curr));

    // use p control to modify constant
    int ret = motor::CENTER + (desired - curr) * motor::K_P;
    // these terms neglible unless near boundary
    ret += pow(motor::E,1 / (motor::C * curr));
    ret -= pow(motor::E, 1 / (motor::C * (motor::POT_MAX - curr)));
    // SerialOut.printf("%d, ", ret);
    // constrain to within bounds
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

    SerialOut.println("Compiled: " __DATE__ ", " __TIME__ );

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


    /** ESTOP **/
    pinMode(pin::ESTOP, INPUT);
    attachInterrupt(digitalPinToInterrupt(pin::ESTOP), Axo::estopCallback, RISING);

    /** IMUs **/
    // bool shinIMUStarted = m_shinIMU.begin();
    // bool footIMUStarted = m_footIMU.begin();

    // if (!shinIMUStarted) {
    //     SerialOut.println("\nERR,Error connecting to shin IMU. Foot IMU status unknown.");
    //     while (1);
    // }
    // if (!footIMUStarted) {
    //     SerialOut.println("\nERR,Error connecting to foot IMU. Shin IMU successfully connected.");
    //     while (1);
    // }

    // delay(1000);

    // // set up external crystal for optimal performance
    // m_shinIMU.setExtCrystalUse(true);
    // m_footIMU.setExtCrystalUse(true);

    // // set IMU offsets
    // if (useIMUOffsets) {
    //     m_shinIMU.setSensorOffsets(offsets::shin);
    //     m_footIMU.setSensorOffsets(offsets::foot);
    // }

    // // Switch to NDOF mode--use all 9 DOFs, 100 Hz fusion data
    // m_shinIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
    // m_footIMU.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);


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

    // DISABLE IMU
    // if (m_timerIMU.check()) {
    //     // Calibration status
    //     m_shinIMU.getCalibration(&m_quatCalShin[0], &m_quatCalShin[1], &m_quatCalShin[2], &m_quatCalShin[3]);
    //     m_footIMU.getCalibration(&m_quatCalFoot[0], &m_quatCalFoot[1], &m_quatCalFoot[2], &m_quatCalFoot[3]);

    //     // Quaternions
    //     m_quatShin = m_shinIMU.getQuat();
    //     m_quatFoot = m_footIMU.getQuat();

    //     // For printing: acceleration, etc?
    //     sensors_event_t shinAccel{};
    //     m_shinIMU.getEvent(&shinAccel, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    //     sensors_event_t footAccel{};
    //     m_footIMU.getEvent(&footAccel, Adafruit_BNO055::VECTOR_ACCELEROMETER);

    //     #ifndef SUPPRESS_IMU
    //     printIMUs(&shinAccel, &footAccel);
    //     #endif
    // }

    // DISABLE FSR
    // if (m_timerFSR.check()) {
    //     m_fsrVal = analogRead(pin::FSR);
    //     #ifndef SUPPRESS_FSR
    //     unsigned long t{millis() - m_startTime};
    //     SerialOut.print("\nF ");
    //     writeBytes(t, 4);   writeBytes(m_fsrVal);
    //     #endif
    // }

    if (m_timerLoad.check()) {
        m_loadCellVal = analogRead(pin::LOADCELL);
        #ifndef SUPPRESS_LOAD
        unsigned long t{millis() - m_startTime};
        SerialOut.print("\nL ");
        writeBytes(t, 4);   writeBytes(m_loadCellVal);
        #endif
    }

    if (m_timerMotor.check()) {
        updateMotors();

        // read potentiometers
        int currPotL = analogRead(pin::POT_L);
        int currPotR = analogRead(pin::POT_R);
        SerialOut.printf("%d, %d\n", m_targetAngleR, currPotR + 1023*m_turnsR);
    }

    if (m_estopPressed) {
        // STOP EVERYTHING
        SerialOut.println("--ESTOP--");
        stopMotors();
        detachMotors();
        setBlueLED(HIGH);
        setGreenLED(LOW);

        // pause the loop
        while (1);
    }
}


void Axo::heartbeat() {
    if (m_timerHeartbeat.check()) {
        #ifndef SUPPRESS_LOG
        unsigned long t{millis() - m_startTime};
        SerialOut.printf("\nLOG,%d | Heartbeat\n",t);
        #endif
    }
}


/* Interrupt handler */
void Axo::estopCallback() {
    m_estopPressed = true;
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

    SerialOut.print("\nI ");
    writeBytes(millis() - m_startTime, 4);
    // calibration constants
    writeBytes(m_quatCalShin[0],1); writeBytes(m_quatCalShin[1],1); writeBytes(m_quatCalShin[2],1); writeBytes(m_quatCalShin[3],1);
    writeBytes(m_quatCalFoot[0],1); writeBytes(m_quatCalFoot[1],1); writeBytes(m_quatCalFoot[2],1); writeBytes(m_quatCalFoot[3],1);
    // quaternions
    writeBytes(quatShin_w); writeBytes(quatShin_x); writeBytes(quatShin_y); writeBytes(quatShin_z);
    writeBytes(quatFoot_w); writeBytes(quatFoot_x); writeBytes(quatFoot_y); writeBytes(quatFoot_z);
    // acceleration
    writeBytes(shinAccel_x); writeBytes(shinAccel_y); writeBytes(shinAccel_z);
    writeBytes(footAccel_x); writeBytes(footAccel_y); writeBytes(footAccel_z);
}


void Axo::printKey() {
    SerialOut.print("\nI ,time,shin_cal_tot,shin_cal_gyr,shin_cal_acc,shin_cal_mag | foot_cal_tot,foot_cal_gyr,foot_cal_acc,foot_cal_mag | "
                      "shin_qw,shin_qx,shin_qy,shin_qz | foot_qw,foot_qx,foot_qy,foot_qz | shin_ax,shin_ay,shin_az | foot_ax,foot_ay,foot_az");

    SerialOut.print("\nF ,time | val");
    SerialOut.print("\nL ,time | val");
    SerialOut.print("\nM ,time | potL_cur,potR_cur | potL_adj,potR_adj | targetPotL,targetPotR | motorCmdL,motorCmdR");
    SerialOut.print("\nLOG,time | msg");
    SerialOut.println("\nERR,msg");
}

// converts float to 2 characters
uint16_t Axo::floatToInt(float f, int precision) {
    int16_t i = static_cast<int16_t>(f * pow(10, precision));
    uint16_t ui = static_cast<uint16_t>(i);
    if (i < 0) {
        ui = i + pow(2, 16);
    }
    return ui;
}

void writeBytes(int v, int numBytes) {
    uint8_t b[numBytes]{};
    // split into bytes
    for (int i{0}; i < numBytes; ++i) {
        b[i] = (v & (0xff << 8*i)) >> 8*i;
    }
    // write the values in reverse order
    for (int i{ numBytes-1 }; i >= 0; --i) {
        SerialOut.write(b[i]);
    }
}