/*
Contains main Axo helper class.

Lorenzo Shaikewitz, 8/10/2021
*/
#ifndef AXO_H
#define AXO_H

#include "constants.h"

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

/*
IMUCarrier - IMU data helper class
*/
class IMUCarrier {
public:
    IMUCarrier(uint8_t in_FXOS8700_addr = FXOS8700_I2C_ADDR0, uint8_t in_FXAS21002_addr = FXAS21002_I2C_ADDR0)
               : m_imu(in_FXOS8700_addr, in_FXAS21002_addr), m_filter{}
    {/*does nothing*/}

    bool begin() {
        m_filter.begin(property::IMU_UPDATE_HZ);
        return m_imu.begin();
    }

    bool available() { return m_imu.available(); }

    void update() {
        m_imu.readMotionSensor(m_ax, m_ay, m_az, m_gx, m_gy, m_gz, m_mx, m_my, m_mz);
        m_filter.update(m_gx, m_gy, m_gz, m_ax, m_ay, m_az, m_mx, m_my, m_mz);
        m_filter.getQuaternion(m_quat);
    }

    const float* getQuat() const { return m_quat; }

    // prints out the quaternion.
    const void printQuat() const;

    NXPMotionSense m_imu;
    NXPSensorFusion m_filter;

private:
    float m_ax, m_ay, m_az;
    float m_gx, m_gy, m_gz;
    float m_mx, m_my, m_mz;
    float m_quat[4]{};
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
        m_fileSize{timeToFileSize(runTimeSeconds, 2)},
        m_imuProp(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0),
        m_imuAda(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1)
    { /*Does nothing*/ }

    // filename must be < 7 characters. No extension needed.
    Message begin(String filename = "data_");

    bool propIMUAvail() { return m_imuProp.available(); }
    bool adaIMUAvail() { return m_imuAda.available(); }

    // updates each IMU separately
    void updateAdaIMU();
    void updatePropIMU();
    bool propUpdated() { return m_propUpdated; }
    bool adaUpdated() { return !m_propUpdated; }


    bool saveData();
    // prints most recent data from both IMUs to serial monitor
    void printData();

    const char* getSavefile() const { return m_savefile; }


private:
    bool addBothQuatToBuf();
    // TODO: addRelQuatToBuf();
    bool addCharToBuf(char c);  // does buf size checking, calls saveFromBuf if buf full
    bool saveFromBuf();

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
};

#endif
