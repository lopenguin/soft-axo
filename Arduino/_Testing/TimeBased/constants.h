/*
Contains all useful constants (pins, file sizes, ...) for Axo.

Lorenzo Shaikewitz, 8/10/2021
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

namespace parameter {
    constexpr int STEP_TIME_MS{1000};

    constexpr float START_PERCENT{0.2};
    constexpr float END_PERCENT{0.6};
    constexpr int MAX_TURN{90};

    const float START_TIME_MS{START_PERCENT * STEP_TIME_MS};
    const float END_TIME_MS{END_PERCENT * STEP_TIME_MS};
}

namespace property {
    constexpr unsigned int FLASH_MAX_PAGES{32768};
    constexpr int FLASH_PAGE_SIZE{256};
    constexpr int QUAT_NUM_DECIMALS{4};
    constexpr int QUAT_DATA_SIZE{2*4 + 1}; // two bytes per quat number, 1 byte for time dif.
    constexpr int IMU_UPDATE_HZ{100};
    constexpr int NUM_QUATS{1};

    constexpr int FILENAME_MAX_LEN{10};

    constexpr float STARTUP_ACCEL_THRESH{1.5};
}

namespace pin {
    constexpr short FLASH_CS{6};

    constexpr short MOTOR_R{3};
    constexpr short MOTOR_L{4};

    constexpr short POT_L{A7};
    constexpr short POT_R{A8};
}

namespace motor {
    constexpr float POT_TO_DEG{ 343.0 / 1023.0 };

    constexpr int MIDDLE_POINT = 1500;
    constexpr int PWM_LOW = 1000;
    constexpr int PWM_HIGH = 2000;
    constexpr int MAX_FORWARD_SPEED = 1560;
    constexpr int MAX_BACKWARD_SPEED = 1440;

    constexpr int CALIB_DELAY = 7000;

    // how close to get to goal in degrees (+/-)
    constexpr int UNCERTAINTY = 10 / POT_TO_DEG;
}

enum class Message {
    OK,
    NO_FLASH_CHIP,
    NO_FLASH_SPACE,
    FILE_TOO_LONG,
};

#endif
