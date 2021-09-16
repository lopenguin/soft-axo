/*
Contains all useful constants (pins, file sizes, ...) for Axo.

Lorenzo Shaikewitz, 8/10/2021
*/
#ifndef AXO_CONSTANTS_H
#define AXO_CONSTANTS_H

namespace property {
    constexpr unsigned int FLASH_MAX_PAGES{32768};
    constexpr int FLASH_PAGE_SIZE{256};
    constexpr int QUAT_NUM_DECIMALS{4};
    constexpr int QUAT_DATA_SIZE{(QUAT_NUM_DECIMALS+1)*2};   // each digit represented by 1 byte
    constexpr int IMU_UPDATE_HZ{100};
    constexpr int NUM_QUATS{1};

    constexpr int FILENAME_MAX_LEN{10};
}

namespace pin {
    constexpr short FLASH_CS{6};

    const short MOTOR_IN1{3};
    const short MOTOR_IN2{4};

    const short MOTOR_EN{5};

    const short JOY_BUTTON{21};
}

enum class Message {
    OK,
    NO_FLASH_CHIP,
    NO_FLASH_SPACE,
    FILE_TOO_LONG,
};

#endif
