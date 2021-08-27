/*
Code

Lorenzo Shaikewitz, 8/26/2021
*/
#include "IMUCarrier.h"
#include "constants.h"

#include <Arduino.h>
#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>


const void IMUCarrier::printQuat() const {
    Serial.print(m_quat[0],4);
    Serial.print(',');
    Serial.print(m_quat[1],4);
    Serial.print(',');
    Serial.print(m_quat[2],4);
    Serial.print(',');
    Serial.print(m_quat[3],4);
}


void IMUCarrier::getRelQuatWithRotMatrix(IMUCarrier& q2, float* relQuat) const {
    // convert both quaternions into rotation matricies
    struct Matrix {
        float i11, i12, i13;
        float i21, i22, i23;
        float i31, i32, i33;
    };
    // this one is transposed
    Matrix m1{};
    {
        float w = this->m_quat[0];
        float x = this->m_quat[1];
        float y = this->m_quat[2];
        float z = this->m_quat[3];

        m1.i11 = 1 - 2*y*y - 2*z*z;
        m1.i21 = 2*x*y - 2*w*z;
        m1.i31 = 2*x*z + 2*w*y;

        m1.i12 = 2*x*y + 2*w*z;
        m1.i22 = 1 - 2*x*x - 2*z*z;
        m1.i32 = 2*y*z - 2*w*x;

        m1.i13 = 2*x*z - 2*w*y;
        m1.i23 = 2*y*z + 2*w*x;
        m1.i33 = 1 - 2*x*x - 2*y*y;
    }
    // this one is normal
    Matrix m2{};
    {
        float w = q2.m_quat[0];
        float x = q2.m_quat[1];
        float y = q2.m_quat[2];
        float z = q2.m_quat[3];

        m2.i11 = 1 - 2*y*y - 2*z*z;
        m2.i12 = 2*x*y - 2*w*z;
        m2.i13 = 2*x*z + 2*w*y;

        m2.i21 = 2*x*y + 2*w*z;
        m2.i22 = 1 - 2*x*x - 2*z*z;
        m2.i23 = 2*y*z - 2*w*x;

        m2.i31 = 2*x*z - 2*w*y;
        m2.i32 = 2*y*z + 2*w*x;
        m2.i33 = 1 - 2*x*x - 2*y*y;
    }

    // multiply the two matricies
    Matrix rel{};
    rel.i11 = m1.i11*m2.i11 + m1.i12*m2.i21 + m1.i13*m2.i31;
    rel.i12 = m1.i11*m2.i12 + m1.i12*m2.i22 + m1.i13*m2.i32;
    rel.i13 = m1.i11*m2.i13 + m1.i12*m2.i23 + m1.i13*m2.i33;

    rel.i21 = m1.i21*m2.i11 + m1.i22*m2.i21 + m1.i23*m2.i31;
    rel.i22 = m1.i21*m2.i12 + m1.i22*m2.i22 + m1.i23*m2.i32;
    rel.i23 = m1.i21*m2.i13 + m1.i22*m2.i23 + m1.i23*m2.i33;

    rel.i31 = m1.i31*m2.i11 + m1.i32*m2.i21 + m1.i33*m2.i31;
    rel.i32 = m1.i31*m2.i12 + m1.i32*m2.i22 + m1.i33*m2.i32;
    rel.i33 = m1.i31*m2.i13 + m1.i32*m2.i23 + m1.i33*m2.i33;


    // convert rel back to a quat (oh boy)
    float trace = rel.i11 + rel.i22 + rel.i33;
    if (trace > 0) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        relQuat[0] = 0.25f / s;
        relQuat[1] = (rel.i32 - rel.i23) * s;
        relQuat[2] = (rel.i13 - rel.i31) * s;
        relQuat[3] = (rel.i21 - rel.i12) * s;
    } else {
        if (rel.i11 > rel.i22 && rel.i11 > rel.i33) {
            float s = 2.0f * sqrtf(1.0f + rel.i11 - rel.i22 - rel.i33);
            relQuat[0] = (rel.i32 - rel.i23) / s;
            relQuat[1] = 0.25f * s;
            relQuat[2] = (rel.i12 + rel.i21) / s;
            relQuat[3] = (rel.i13 + rel.i31) / s;
        } else if (rel.i22 > rel.i33) {
            float s = 2.0f * sqrtf(1.0f + rel.i22 - rel.i11 - rel.i33);
            relQuat[0] = (rel.i13 - rel.i31) / s;
            relQuat[1] = (rel.i12 + rel.i21) / s;
            relQuat[2] = 0.25f * s;
            relQuat[3] = (rel.i23 + rel.i32) / s;
        } else {
            float s = 2.0f * sqrtf(1.0f + rel.i33 - rel.i11 - rel.i22);
            relQuat[0] = (rel.i21 - rel.i12) / s;
            relQuat[1] = (rel.i13 + rel.i31) / s;
            relQuat[2] = (rel.i23 + rel.i32) / s;
            relQuat[3] = 0.25f * s;
        }
    }
}



void IMUCarrier::getRelQuat(IMUCarrier& q2, float* relQuat) const {
    relQuat[0] = (this->m_quat[0])*q2.m_quat[0]  - (-this->m_quat[1])*q2.m_quat[1]
               - (-this->m_quat[2])*q2.m_quat[2] - (-this->m_quat[3])*q2.m_quat[3];

    relQuat[1] = (this->m_quat[0])*q2.m_quat[1]  + (-this->m_quat[1])*q2.m_quat[0]
               + (-this->m_quat[2])*q2.m_quat[3] - (-this->m_quat[3])*q2.m_quat[2];

    relQuat[2] = (this->m_quat[0])*q2.m_quat[2]  - (-this->m_quat[1])*q2.m_quat[3]
               + (-this->m_quat[2])*q2.m_quat[0] + (-this->m_quat[3])*q2.m_quat[1];


    relQuat[3] = (this->m_quat[0])*q2.m_quat[3]  + (-this->m_quat[1])*q2.m_quat[2]
               - (-this->m_quat[2])*q2.m_quat[1] + (-this->m_quat[3])*q2.m_quat[0];
}
