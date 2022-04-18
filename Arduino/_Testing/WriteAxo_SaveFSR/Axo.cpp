/*

Lorenzo Shaikewitz, 8/10/2021
*/
#include "IMUCarrier.h"
#include "Axo.h"
#include "constants.h"

#include <Arduino.h>
#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>


void Axo::begin() {
    // just sets up IMUs and motors
    // IMU setup
    m_imuProp.begin();
    m_imuAda.begin();
}

Message Axo::beginFlash(String quatFilename, String fsrFilename) {
    if (quatFilename.length() > (property::FILENAME_MAX_LEN + 2)) {
        return Message::FILE_TOO_LONG;
    }
    if (fsrFilename.length() > (property::FILENAME_MAX_LEN + 2)) {
        return Message::FILE_TOO_LONG;
    }

    // flash setup
    if (!SerialFlash.begin(pin::FLASH_CS)) {
        return Message::NO_FLASH_CHIP;
    }
    String tryfile = quatFilename + "00";
    String tryfileFSR = fsrFilename;
    int number = 0;
    while (SerialFlash.exists(tryfile.c_str())) {
        // increment the number
        ++number;
        String add;
        if (number < 10)
            add = "0" + String(number);
        else
            add = String(number);
        tryfile = quatFilename + add;
        tryfileFSR = fsrFilename + add;
    }
    tryfile.toCharArray(m_quatSavefile, tryfile.length() + 1);
    tryfileFSR.toCharArray(m_fsrSavefile, tryfileFSR.length() + 1);

    // create save file
    if (!SerialFlash.create(m_quatSavefile, m_quatFileSize)) {
        return Message::NO_FLASH_SPACE;
    }

    // create save file
    if (!SerialFlash.create(m_fsrSavefile, m_fsrFileSize)) {
        return Message::NO_FLASH_SPACE;
    }

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


bool Axo::started() {
    if (m_imuProp.getAz() > property::STARTUP_ACCEL_THRESH) {
        return true;
    }
    return false;
}


bool Axo::saveData(unsigned long timeDif) {
    // bool flashHasSpace = addBothQuatToBuf();
    bool flashHasSpace = addTimeToBuf(timeDif);
    flashHasSpace = flashHasSpace && addRelQuatToBuf();
    if (m_quatBufIndex >= property::FLASH_PAGE_SIZE) {
        flashHasSpace = saveFromBuf();
        m_quatBufIndex = 0;
    }
    return flashHasSpace;
}


bool Axo::saveFSR(unsigned long timeDif) {
    // same time first
    bool flashHasSpace = addTimeToBuf(timeDif, m_fsrSavefile, m_fsrBuf, m_fsrBufIndex, m_currentPosFSR);
    flashHasSpace = flashHasSpace && addFSRValToBuf();
    if (m_fsrBufIndex >= property::FLASH_PAGE_SIZE) {
        flashHasSpace = saveFromBuf(m_fsrSavefile, m_fsrBuf, m_currentPosFSR);
        m_fsrBufIndex = 0;
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


void Axo::printRelQuat() {
    Serial.print("Rel: ");
    Serial.print(m_relQuat[0]);
    Serial.print(',');
    Serial.print(m_relQuat[1]);
    Serial.print(',');
    Serial.print(m_relQuat[2]);
    Serial.print(',');
    Serial.print(m_relQuat[3]);
    Serial.print('\n');
}

bool Axo::FSRStepped() {
    bool returnVal{0};
    // read FSR
    int fsrVal{analogRead(pin::FSR)};
    if (fsrVal > property::FSR_THRESH) {
        if (!m_fsrHigh) {
            // let user know they stepped
            returnVal = 1;
        }
        m_fsrHigh = 1;
    } else {
        m_fsrHigh = 0;
    }
    return returnVal;
}


bool Axo::addTimeToBuf(unsigned long t) {
    return addTimeToBuf(t, m_quatSavefile, m_quatBuf, m_quatBufIndex, m_currentPosQuat);
}


bool Axo::addTimeToBuf(unsigned long t, char* savefile, char* buf, int& bufIdx, int& currentPos) {
    // time comes in microseconds, convert to millis * 10 and use 8 bit.
    uint8_t timeDif{};
    if (t/100 > 255) {
        timeDif = 255;
    } else {
        timeDif = static_cast<uint8_t>(t/100);
    }
    bool flashHasSpace = addCharToBuf(timeDif, savefile, buf, bufIdx, currentPos);
    return flashHasSpace;
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
        [this](const float* quat) -> bool {
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

    return saveQuat(quatProp) && saveQuat(quatAda);
}


bool Axo::addRelQuatToBuf() {
    // save in binary
    m_imuProp.getRelQuat(m_imuAda, m_relQuat);

    bool flashHasSpace = 1;
    for (int i{0}; i < 4; ++i) {
        // convert from +x.xxxx to (xxxxx + 1 * 10000) > 0
        // writing is in bytes: 1 character is 1 byte.
        // range: (0, 20000). 1 byte can store up to 2^8 = 256.
        // Two bytes can store up to 2^16 = 65536. Let's use two bytes per number.
        int q = static_cast<int>((m_relQuat[i]+1) * pow(10, property::QUAT_NUM_DECIMALS));
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


bool Axo::addFSRValToBuf() {
    int fsrVal = analogRead(pin::FSR);

    bool flashHasSpace = 1;
    // convert from 0->1023 into uint8
    uint8_t c = map(fsrVal, 0, 1023, 0, 255);

    // write it!
    flashHasSpace = addCharToBuf(c, m_fsrSavefile, m_fsrBuf, m_fsrBufIndex, m_currentPosFSR);
    return flashHasSpace;
}


bool Axo::addCharToBuf(char c) {
    return addCharToBuf(c, m_quatSavefile, m_quatBuf, m_quatBufIndex, m_currentPosQuat);
}


bool Axo::addCharToBuf(char c, char* savefile, char* buf, int& bufIdx, int& currentPos) {
    bool flashHasSpace = 1;
    if (bufIdx >= property::FLASH_PAGE_SIZE) {
        flashHasSpace = saveFromBuf(savefile, buf, currentPos);
        bufIdx = 0;
    }

    buf[bufIdx] = c;
    ++bufIdx;

    return flashHasSpace;
}


bool Axo::saveFromBuf() {
    return saveFromBuf(m_quatSavefile, m_quatBuf, m_currentPosQuat);
}

bool Axo::saveFromBuf(char* savefile, char* buf, int& currentPos) {
    int bufSize{property::FLASH_PAGE_SIZE};

    SerialFlashFile file = SerialFlash.open(savefile);

    file.seek(currentPos);
    if (!file.write(buf, bufSize)) {
        file.close();
        return false;
    }

    currentPos = file.position();
    file.flush();
    return true;
}


int Axo::timeToQuatFileSize(int runTimeSeconds, int numQuats) {
    int numDataPoints = runTimeSeconds * property::IMU_UPDATE_HZ;
    unsigned long fileSizeNeeded = (numDataPoints * property::QUAT_DATA_SIZE * numQuats);
    if (fileSizeNeeded > property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE)
        fileSizeNeeded = property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE;
    return fileSizeNeeded;
}


int Axo::timeToFSRFileSize(int runTimeSeconds) {
    // assume we record one step per second, at most
    int numDataPoints = runTimeSeconds * 1;
    // FSR requires 1 byte for time, 1 byte for binary value (2 bytes total)
    unsigned long fileSizeNeeded = (numDataPoints * 2);
    if (fileSizeNeeded > property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE)
        fileSizeNeeded = property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE;
    return fileSizeNeeded;
}
