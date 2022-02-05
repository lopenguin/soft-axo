/*
Main Axo helper class. Carries both IMUs, handles low-level data saving and calculations.

Lorenzo Shaikewitz, 8/10/2021
Up to date as of 1/17/2022
*/
#ifndef AXO_H
#define AXO_H

#include "IMUCarrier.h"
#include "constants.h"

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>


class IMUHistory {
public:
    IMUHistory() : m_relQuatHistory{}
    {m_nextIdx = 0;}

    void add(float quat);

    // computes slope of best fit line
    float getSlope();


private:
    static const int m_size{30};
    float m_relQuatHistory[m_size]{};

    int m_nextIdx{};

};


/*
Axo - carrier class
* Encapulates motors, sensors, data logging, and control

Note: using motors not yet implemented.
*/
class Axo {
public:
    // defaults to passive, 5 minute recording time
    Axo(int runTimeSeconds = 300, bool useMotors = false) :
        m_useMotors{useMotors},
        m_fileSize{timeToFileSize(runTimeSeconds, property::NUM_QUATS)},
        m_imuProp(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0),
        m_imuAda(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1),
        m_history()
    { /*Does nothing*/ }

    // starts up IMU and motors but not flash file.
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

    // deals with averaging
    bool updateAverage();

    const char* getSavefile() const { return m_savefile; }


private:
    bool addBothQuatToBuf();
    bool addRelQuatToBuf();
    bool addCharToBuf(char c);  // does buf size checking, calls saveFromBuf if buf full
    bool saveFromBuf();
    bool addTimeToBuf(unsigned long t);

    int timeToFileSize(int runTimeSeconds, int numQuats = 2); // UNTESTED!!

    bool m_useMotors{};
    const long m_fileSize{};
    char m_savefile[property::FILENAME_MAX_LEN]{};
    int m_currentPos{0};

    char m_quatBuf[property::FLASH_PAGE_SIZE]{};
    int m_quatBufIndex{};
    bool m_propUpdated{0};

    IMUCarrier m_imuProp;
    IMUCarrier m_imuAda;
    float m_relQuat[4]{};

    IMUHistory m_history;
    float m_lastSlope{};
};

#endif
