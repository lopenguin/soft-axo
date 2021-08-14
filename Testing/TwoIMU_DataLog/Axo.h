/*
Contains main Axo helper class.

Lorenzo Shaikewitz, 8/10/2021
*/
#ifndef AXO_H
#define AXO_H

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

    bool available() { return m_imu.available(); }

    void update() {
        m_imu.readMotionSensor(m_ax, m_ay, m_az, m_gx, m_gy, m_gz, m_mx, m_my, m_mz);
        m_filter.update(m_gx, m_gy, m_gz, m_ax, m_ay, m_az, m_mx, m_my, m_mz);
        m_filter.getQuaternion(m_quat);
    }

    const float* getQuat() const { return m_quat; }

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
    Axo(bool useMotors = false) :
        m_useMotors{useMotors},
        m_fileSize{timeToFileSize(runTimeSeconds)}
    { /*Does nothing*/ }

    // filename must be < 7 characters. No extension needed.
    Message begin(String filename = "data_");

    // updates both IMUs together, saves data to buffer
    void updateIMUs();

    const char* getSavefile() const { return m_savefile; }


private:
    void addBothQuatToBuf();
    // TODO: addRelQuatToBuf();
    bool saveFromBuf(char* buf, const int bufSize);
    int timeToFileSize(int runTimeSeconds); // UNTESTED!!

    bool m_useMotors{};
    const int m_fileSize{};
    char m_savefile[10]{};
    int m_currentPos{0};

    IMUCarrier m_imuProp(FXOS8700_I2C_ADDR0, FXAS21002_I2C_ADDR0);
    IMUCarrier m_imuAda(FXOS8700_I2C_ADDR3, FXAS21002_I2C_ADDR1);
};

#endif
