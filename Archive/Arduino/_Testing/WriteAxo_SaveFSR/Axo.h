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

/*
Axo - carrier class
* Encapulates motors, sensors, data logging, and control

Note: using motors not yet implemented.
*/
class Axo {
public:
    // defaults to passive, 5 minute recording time
    Axo(int runTimeSeconds = 300) :
        m_quatFileSize{timeToQuatFileSize(runTimeSeconds, property::NUM_QUATS)},
        m_fsrFileSize{timeToFSRFileSize(runTimeSeconds)},
        m_imuProp(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0),
        m_imuAda(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1)
    { /*Does nothing*/ }

    // starts up IMU and motors but not flash file.
    void begin();

    // filename must be < 7 characters. No extension needed.
    Message beginFlash(String quatFilename, String fsrFilename);

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
    bool saveFSR(unsigned long timeDif);

    // prints most recent data from both IMUs to serial monitor
    void printData();
    void printRelQuat();

    // deals with averaging
    // bool updateAverage();
    // handles FSR step detection
    bool FSRStepped();

    const char* getQuatSavefile() const { return m_quatSavefile; }
    const char* getFSRSavefile() const { return m_fsrSavefile; }


private:
    bool addBothQuatToBuf();
    bool addRelQuatToBuf();
    bool addCharToBuf(char c);  // does buf size checking, calls saveFromBuf if buf full
    bool saveFromBuf();
    bool addTimeToBuf(unsigned long t);

    // general version
    bool addCharToBuf(char c, char* savefile, char* buf, int& bufIdx, int& currentPos);
    bool saveFromBuf(char* savefile, char* buf, int& currentPos);
    bool addFSRTimeToBuf(unsigned long t, char* savefile, char* buf, int& bufIdx, int& currentPos);
    bool addFSRValToBuf();

    int timeToQuatFileSize(int runTimeSeconds, int numQuats = 2);
    int timeToFSRFileSize(int runTimeSeconds);

    const long m_quatFileSize{};
    const long m_fsrFileSize{};
    char m_quatSavefile[property::FILENAME_MAX_LEN]{};
    char m_fsrSavefile[property::FILENAME_MAX_LEN]{};
    int m_currentPosQuat{0};
    int m_currentPosFSR{0};

    char m_fsrBuf[property::FLASH_PAGE_SIZE]{};
    int m_fsrBufIndex{};

    char m_quatBuf[property::FLASH_PAGE_SIZE]{};
    int m_quatBufIndex{};
    bool m_propUpdated{0};

    IMUCarrier m_imuProp;
    IMUCarrier m_imuAda;
    float m_relQuat[4]{};

    // IMUHistory m_history;
    // float m_lastSlope{};

    bool m_fsrHigh{0};
};

#endif
