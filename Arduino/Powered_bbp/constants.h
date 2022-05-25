/*
Holds constants for Axo operations.

Lorenzo Shaikewitz, 4/17/2022
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

/*Serial definition*/
// #define SerialOut Serial    // for serial monitor
#define SerialOut Serial1   // for Wixel streaming

/*Side definition (for IMU offsets)*/
#define USE_ZERO_OFFSETS
// #define USE_ONE_OFFSETS

/*Suppression definition*/
// #define SUPPRESS_IMU
// #define SUPPRESS_FSR
// #define SUPPRESS_LOAD
// #define SUPPRESS_MOTOR
// #define SUPPRESS_LOG

constexpr int BAUD{115200};

namespace control {
    constexpr int FSR_THRESH{450};
}

namespace pin {
    // ESCs
    constexpr short ESC_L{4};
    constexpr short ESC_R{3};

    // sensors
    constexpr short FSR{16};
    constexpr short LOADCELL{17};
    constexpr short POT_L{15};
    constexpr short POT_R{14};

    // LEDs and e-stop
    constexpr short LEDB{6};
    constexpr short LEDG{7};
    constexpr short ESTOP{23};
}


namespace timer {
    constexpr unsigned int IMU{10}; // ms
    constexpr unsigned int FSR{10}; // ms (TODO good rate??)
    constexpr unsigned int LOADCELL{10}; // ms (TODO good rate??)
    constexpr unsigned int HEARTBEAT{2000}; // ms

    constexpr unsigned int MOTOR{10}; // ms (TODO: faster??)

    constexpr unsigned int SERIAL_TIMEOUT{500}; // ms
}


namespace motor {
    constexpr unsigned int CAL_PWM{1500};
    constexpr unsigned int CAL_DELAY{7000}; // ms

    constexpr int NOISE_JUMP = 500;         // minimum jump in potentiometer values to define a whole turn
    constexpr int POT_MAX{ 1023 };
    constexpr int POT_MIN{ 0 };

    // (low-level) CONTROL CONSTANTS
    constexpr double C = 0.005;             // see Desmos graph for intuition for C
    constexpr double E = 2.71828;           // the natural constant e
    constexpr double K_P = 0.7;             // proportional control constant
    constexpr int MAX_FORWARD = 1600;       // maximum permitted forward PWM (m_f)
    constexpr int MAX_BACKWARD = 1400;      // maximum permitted backward PWM (m_b)
    constexpr int CENTER = 1500;
    constexpr int LOWMS_THRESHOLD{20};

    // left and right motor constants
    constexpr int LZERO{923};
    constexpr int RZERO{100};
}


namespace offsets {
    #ifdef USE_ZERO_OFFSETS
    constexpr adafruit_bno055_offsets_t shin{
        -7, -25, 1, // accel offsets
        325, 185, -92, // mag offsets
        0, -1, -1, // gyro offsets
        1000,         // accel radius
        800          // mag radius
    };


    constexpr adafruit_bno055_offsets_t foot{
        -21, -49, -54, // accel offsets
        14, 324, -124, // mag offsets
        -1, -1, 0, // gyro offsets
        1000,         // accel radius
        807          // mag radius
    };
    #endif

    #ifdef USE_ONE_OFFSETS
    constexpr adafruit_bno055_offsets_t shin{
        3, -86, -15, // accel offsets
        -262, 487, -434, // mag offsets
        -2, 2, 0, // gyro offsets
        1000,         // accel radius
        788          // mag radius
    };


    constexpr adafruit_bno055_offsets_t foot{
        -25, -57, -43, // accel offsets
        -338, 303, -115, // mag offsets
        1, -5, 0, // gyro offsets
        1000,         // accel radius
        879          // mag radius
    };
    #endif
}

#endif
