/*
Main Axo helper class. Carries both IMUs, handles low-level data saving and calculations.

Lorenzo Shaikewitz, 8/10/2021
Up to date as of 1/17/2022
*/
#ifndef AXO_H
#define AXO_H

#include "IMUCarrier.h"
#include "DroneMotor.h"
#include "constants.h"

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

/*
Axo - carrier class
* Encapulates motors, sensors, data logging, and control

Note: using motors not yet implemented.
*/
class Axo {
public:
    // defaults to passive, 5 minute recording time
    Axo(int runTimeSeconds = 300) :
        m_fileSize{timeToFileSize(runTimeSeconds, property::NUM_QUATS)},
        m_imuProp(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0),
        m_imuAda(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1),
        m_motorL(pin::MOTOR_L, pin::POT_L),
        m_motorR(pin::MOTOR_R, pin::POT_R)
    { /*Does nothing*/ }

    // starts up IMU and motors but not flash file. (takes ~7 seconds)
    void begin();

    // filename must be < 7 characters. No extension needed.
    Message beginFlash(String filename = "data_");

    bool propIMUAvail() { return m_imuProp.available(); }
    bool adaIMUAvail() { return m_imuAda.available(); }

    // updates each IMU separately
    void updateAdaIMU();
    void updatePropIMU();
    bool propUpdated() { return m_propUpdated; }
    bool adaUpdated() { return !m_propUpdated; }

    // startup
    bool started();

    bool saveData(unsigned long timeDif);
    // prints most recent data from both IMUs to serial monitor
    void printData();
    void printRelQuat();

    const char* getSavefile() const { return m_savefile; }

    // runs motors (range between 0 - 343 degrees). Returns true when done
    bool setMotorAngle(float angle);

    // runs the motors at a fixed speed, with a hardstop included at a given angle
    void setMotorAngleWithSpeed(float startAngle, float endAngle, int offset);

    // writes the motors to a speed given by 1500 +/- offset.
    // directly uses writeMicroseconds
    void writeMotors(int offset);

    int readRPot() { return m_motorR.readPot(); }
    int readLPot() { return m_motorL.readPot(); }

    void centerMotors();


private:
    bool addBothQuatToBuf();
    bool addRelQuatToBuf();
    bool addCharToBuf(char c);  // does buf size checking, calls saveFromBuf if buf full
    bool saveFromBuf();
    bool addTimeToBuf(unsigned long t);

    int timeToFileSize(int runTimeSeconds, int numQuats = 2); // UNTESTED!!

    const long m_fileSize{};
    char m_savefile[property::FILENAME_MAX_LEN]{};
    int m_currentPos{0};

    char m_quatBuf[property::FLASH_PAGE_SIZE]{};
    int m_quatBufIndex{};
    bool m_propUpdated{0};

    IMUCarrier m_imuProp;
    IMUCarrier m_imuAda;
    float m_relQuat[4]{};

    DroneMotor m_motorR;
    DroneMotor m_motorL;
    const int m_startingPosL{270};
    const int m_startingPosR{90};
};

#endif
