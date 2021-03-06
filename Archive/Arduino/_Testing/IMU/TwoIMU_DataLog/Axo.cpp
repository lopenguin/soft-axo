/*

Lorenzo Shaikewitz, 8/10/2021
*/
#include "Axo.h"
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


Message Axo::begin(String filename) {
    if (filename.length() > (property::FILENAME_MAX_LEN + 2)) {
        return Message::FILE_TOO_LONG;
    }

    // IMU setup
    m_imuProp.begin();
    m_imuAda.begin();

    // flash setup
    if (!SerialFlash.begin(pin::FLASH_CS)) {
        return Message::NO_FLASH_CHIP;
    }
    String tryfile = filename + "00";
    int number = 0;
    while (SerialFlash.exists(tryfile.c_str())) {
        // increment the number
        ++number;
        String add;
        if (number < 10)
            add = "0" + String(number);
        else
            add = String(number);
        tryfile = filename + add;
    }
    tryfile.toCharArray(m_savefile, tryfile.length() + 1);

    // create save file
    if (!SerialFlash.create(m_savefile, m_fileSize)) {
        return Message::NO_FLASH_SPACE;
    }

    // if (useMotors) {
    //     // Motor setup (not yet implemented)
    // }

    return Message::OK;
}


void Axo::updateAdaIMU() {
    m_propUpdated = 0;
    m_imuAda.update();
}

void Axo::updatePropIMU() {
    m_propUpdated = 1;
    m_imuProp.update();
}


bool Axo::saveData() {
    bool flashHasSpace = addBothQuatToBuf();
    if (m_quatBufIndex >= property::FLASH_PAGE_SIZE) {
        flashHasSpace = saveFromBuf();
        m_quatBufIndex = 0;
    }
    return flashHasSpace;
}


void Axo::printData() {
    Serial.print("Prop: ");
    m_imuProp.printQuat();
    Serial.print('\t');
    Serial.print("Ada: ");
    m_imuAda.printQuat();
    Serial.print('\n');
}


bool Axo::addBothQuatToBuf() {
    // each quat is 6 chars: +x.xxx (x4) without any formatting
    // formatting: +1 (always positive), drop decimal.
    // +x.xxx -> xxxx (4 chars)

    // separated by ',' and terminated by '\n' (+4 chars)
    // total: 24/quat (see constants.h for potentially updated number)
    // property::QUAT_DATA_SIZE

    // lambda to do the saving.
    auto saveQuat{
        [this](const float* quat, bool end) -> bool {
            bool flashHasSpace = 1;
            for (int i{0}; i < 4; ++i) {
                // convert from +x.xxxx to (xxxxx + 1 * 10000) > 0
                // writing is in bytes: 1 character is 1 byte.
                // range: (0, 20000). 1 byte can store up to 2^8 = 256.
                // Two bytes can store up to 2^16 = 65536. Let's use two bytes
                int q = static_cast<int>((quat[i]+1) * pow(10, property::QUAT_NUM_DECIMALS));
                // Serial.print(q);
                // Serial.print('\t');

                uint8_t c1, c2;

                if (q - 0xFF <= 0) {
                    c1 = 0x00;
                    c2 = q;
                } else {
                    c1 = q & 0xFF;
                    c2 = q >> 8;
                }
                // Serial.print(c2);
                // Serial.print(' ');
                // Serial.println(c1);

                // write it!
                addCharToBuf(c2);
                flashHasSpace = addCharToBuf(c1);
            }
            return flashHasSpace;
        }
    };

    const float* quatProp = m_imuProp.getQuat();
    const float* quatAda = m_imuAda.getQuat();

    return saveQuat(quatProp, 0) && saveQuat(quatAda, 1);
}


bool Axo::addCharToBuf(char c) {
    bool flashHasSpace = 1;
    if (m_quatBufIndex >= property::FLASH_PAGE_SIZE) {
        flashHasSpace = saveFromBuf();
        m_quatBufIndex = 0;
    }

    m_quatBuf[m_quatBufIndex] = c;
    ++m_quatBufIndex;

    return flashHasSpace;
}


bool Axo::saveFromBuf() {
    int bufSize{property::FLASH_PAGE_SIZE};

    SerialFlashFile file = SerialFlash.open(m_savefile);

    file.seek(m_currentPos);
    if (!file.write(m_quatBuf, bufSize)) {
        file.close();
        return false;
    }

    m_currentPos = file.position();
    file.flush();
    return true;
}


int Axo::timeToFileSize(int runTimeSeconds, int numQuats) {
    int numDataPoints = runTimeSeconds * property::IMU_UPDATE_HZ;
    long fileSizeNeeded = (numDataPoints * property::QUAT_DATA_SIZE * numQuats);
    if (fileSizeNeeded > property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE)
        fileSizeNeeded = property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE;
    return fileSizeNeeded;
}
