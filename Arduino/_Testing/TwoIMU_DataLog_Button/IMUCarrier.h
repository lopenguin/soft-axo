/*
A class to read, store, and manipulate IMU data.

Lorenzo Shaikewitz, 8/26/2021
*/
#ifndef IMUCARRIER_H
#define IMUCARRIER_H

#include "constants.h"

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>


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

    // calculates relative quaternion using formula:
    // R = A^t * B, where R, A, B are rotation matricies
    // see http://www.songho.ca/opengl/gl_quaternion.html for the transform
    // and https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    // to convert back to quats
    void getRelQuatWithRotMatrix(IMUCarrier& q2, float* relQuat) const;

    // well. Math suggests quaternion left multiplication does same thing!
    void getRelQuat(IMUCarrier& q2, float* relQuat) const;

    NXPMotionSense m_imu;
    NXPSensorFusion m_filter;

private:
    float m_ax, m_ay, m_az;
    float m_gx, m_gy, m_gz;
    float m_mx, m_my, m_mz;
    float m_quat[4]{};
};


#endif
