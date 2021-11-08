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
    constexpr int QUAT_DATA_SIZE{2*4 + 1}; // two bytes per quat number, 1 byte for time dif.
    constexpr int IMU_UPDATE_HZ{100};
    constexpr int NUM_QUATS{1};

    constexpr int FILENAME_MAX_LEN{10};

    constexpr int STARTUP_ACCEL_THRESH{1.5};
}

namespace pin {
    constexpr short FLASH_CS{6};
}

enum class Message {
    OK,
    NO_FLASH_CHIP,
    NO_FLASH_SPACE,
    FILE_TOO_LONG,
};

#endif
