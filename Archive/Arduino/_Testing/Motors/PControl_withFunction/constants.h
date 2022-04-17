#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

namespace motor {
    const float POT_TO_DEG{ 343.0 / 1023.0 };

    const int MIDDLE_POINT = 1500;
    const int PWM_LOW = 1000;
    const int PWM_HIGH = 2000;
    const int MAX_FORWARD_SPEED = 1800;
    const int MAX_BACKWARD_SPEED = 1200;

    const int MIN_SPEED_BELOW = 1420;
    const int MIN_SPEED_ABOVE = 1580;

    const int CALIB_DELAY = 7000;

    // how close to get to goal in degrees (+/-)
    const int UNCERTAINTY = 10 / POT_TO_DEG;
}

namespace pin {
    const short MOTOR_R{3};
    const short MOTOR_L{4};

    const short POT_L{A7};
    const short POT_R{A8};
}

#endif
