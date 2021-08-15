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
    Serial.print(m_quat[0]);
    Serial.print(',');
    Serial.print(m_quat[1]);
    Serial.print(',');
    Serial.print(m_quat[2]);
    Serial.print(',');
    Serial.print(m_quat[3]);
    Serial.print('\n');
}


Message Axo::begin() {
    // IMU setup
    m_imuProp.begin();
    m_filterProp.begin(property::IMU_UPDATE_HZ);
    m_imuAda.begin();
    m_filterAda.begin(property::IMU_UPDATE_HZ);

    // flash setup
    if (!SerialFlash.begin(pin::FLASH_CS)) {
        return Message::NO_FLASH_CHIP;
    }
    String tryfile = filename + "00";
    int number = 0;
    while (SerialFlash.exists(tryfile.c_str()) {
        // increment the number
        ++number;
        String add;
        if (number < 10)
            add = 0 + String(number);
        else
            add = String(number);
        tryfile = filename + add;
    }
    m_savefile = tryfile.toCharArray();

    // create save file
    if (!SerialFlash.create(m_savefile, m_fileSize)) {
        return Message::NO_FLASH_SPACE;
    }

    // if (useMotors) {
    //     // Motor setup (not yet implemented)
    // }

    return Message::OK;
}


void Axo::updateIMUs() {
    m_imuProp.update();
    m_imuAda.update();

    addBothQuatToBuf();
    if (m_quatBufIndex >= property::FLASH_PAGE_SIZE) {
        saveFromBuf();
        m_quatBufIndex = 0;
    }
}


void Axo::printData() {
    Serial.print("Prop: ");
    m_imuProp.printQuat();
    Serial.print('\t');
    Serial.print("Ada: ");
    m_imuAda.printQuat();
    Serial.print('\n');
}


void Axo::addBothQuatToBuf() {
    // each quat is 5 chars: +x.xx (x4)
    // separated by ',' and terminated by '\n' (+4 chars)
    // total: 24/quat (see constants.h for potentially updated number)
    // property::QUAT_DATA_SIZE

    char quatProp[4] = m_imuProp.getQuat();

    // save the prop
    for (int i{0}; i < 4; ++i) {
        String q = String(quatProp[i],property::QUAT_NUM_DECIMALS);
        const char* q_cStr = q.c_str();

        if (q_cStr[0] == '-') {
            for (int j{ 0 }; j < 7; ++j) {
                addCharToBuf(q_cStr[j]);
            }
        } else {
            addCharToBuf('0');
            for (int j{ 0 }; j < 6; ++j) {
                addCharToBuf(q_cStr[j]);
            }
        }
        if (i != 3)
            addCharToBuf(',');
        else
            addCharToBuf('\n');
    }

    char quatAda[4] = m_imuAda.getQuat();
}


void Axo::addCharToBuf(char c) {
    if (m_quatBufIndex >= property::FLASH_PAGE_SIZE) {
        saveFromBuf();
        m_quatBufIndex = 0;
    }

    m_quatBuf[m_quatBufIndex] = c;
    ++m_quatBufIndex;
}


bool Axo::saveFromBuf(char* buf = m_quatBuf, const int bufSize = property::FLASH_PAGE_SIZE) {
    SerialFlashFile file = SerialFlash.open(m_savefile);

    file.seek(m_currentPos);
    if (!file.write(buf, bufSize)) {
        file.close();
        return false;
    }

    m_currentPos = file.position();
    file.flush();
    return true;
}


int Axo::timeToFileSize(int runTimeSeconds, int numQuats = 2) {
    int numDataPoints = runTimeSeconds * property::IMU_UPDATE_HZ;
    int pagesNeeded = (numDataPoints * property::QUAT_DATA_SIZE * numQuats) / property::FLASH_PAGE_SIZE;
    if (pagesNeeded > property::FLASH_MAX_PAGES)
        pagesNeeded = property::FLASH_MAX_PAGES;
    return pagesNeeded;
}
