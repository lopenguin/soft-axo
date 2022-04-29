/*
Holds constants for Axo operations.

Lorenzo Shaikewitz, 4/17/2022
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

// Serial definition
#define SerialOut Serial    // for serial monitor
// #define SerialOut Serial1   // for Wixel streaming

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

#endif
