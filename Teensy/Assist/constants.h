/*
Constants for Axo
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

// TODO: UPDATE
namespace pin {
    // sensors
    constexpr short FSR_L{16};
    constexpr short FSR_R{16};
    constexpr short LOADCELL_L{17};
    constexpr short LOADCELL_R{17};
}


namespace control {
    constexpr int FSR_HIGH_THRESH{420};
    constexpr int FSR_LOW_THRESH{100};
    constexpr float ALPHA_STEP{0.92};
}

namespace timer {
    constexpr unsigned int FSR{10}; // ms (TODO good rate??)
    constexpr unsigned int LOADCELL{10}; // ms (TODO good rate??)

    constexpr unsigned int MOTOR{1}; // ms (TODO: faster??)

    constexpr unsigned int SERIAL_TIMEOUT{500}; // ms
}

#endif