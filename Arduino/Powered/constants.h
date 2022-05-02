/*
Holds constants for Axo operations.

Lorenzo Shaikewitz, 4/17/2022
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Serial definition
#define SerialOut Serial    // for serial monitor
// #define SerialOut Serial1   // for Wixel streaming

// side definition (for IMU offsets)
#define USE_LEFT_OFFSETS
// #define USE_RIGHT_OFFSETS

constexpr int BAUD{57600};

namespace pin {
    // ESCs
    constexpr short ESC_L{3};
    constexpr short ESC_R{4};

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
    constexpr unsigned int LOADCELL{50}; // ms (TODO good rate??)

    constexpr unsigned int MOTOR_US{10000}; // us (TODO: faster??)
}


namespace motor {
    constexpr unsigned int CAL_PWM{1500};
    constexpr unsigned int CAL_DELAY{7000}; // ms

    constexpr int NOISE_JUMP = 500;         // minimum jump in potentiometer values to define a whole turn
}


namespace offsets {
    #ifdef USE_LEFT_OFFSETS
    adafruit_bno055_offsets_t shin {
        10, -56, -10, // accel offsets
        345, 217, -83, // mag offsets
        0, 0, -2, // gyro offsets
        1000,         // accel radius
        766          // mag radius
    };

    adafruit_bno055_offsets_t foot {
        10, -56, -10, // accel offsets
        345, 217, -83, // mag offsets
        0, 0, -2, // gyro offsets
        1000,         // accel radius
        766          // mag radius
    };
    #endif

    #ifdef USE_RIGHT_OFFSETS
    adafruit_bno055_offsets_t shin {
        10, -56, -10, // accel offsets
        345, 217, -83, // mag offsets
        0, 0, -2, // gyro offsets
        1000,         // accel radius
        766          // mag radius
    };

    adafruit_bno055_offsets_t foot {
        10, -56, -10, // accel offsets
        345, 217, -83, // mag offsets
        0, 0, -2, // gyro offsets
        1000,         // accel radius
        766          // mag radius
    };
    #endif
}

#endif
