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

    // set up motors
    m_motorL.attach();
    m_motorR.attach();
    delay(500);
    m_motorL.center();
    m_motorR.center();
    delay(3000);


    m_motorL.readPot();
    m_motorR.readPot();
    // move to 90 degree position
    int c{0};
    while (1) {
        bool lThere{m_motorL.moveToAngle(m_startingPosL)};
        bool rThere{m_motorR.moveToAngle(m_startingPosR)};

        if (lThere && rThere) {
            Serial.print('\n');
            break;
        }
        Serial.print('\n');

        // delay to emulate 100 Hz
        delay(10);
        c++;
        if (c > 100) {
            break;
        }
    }
}

Message Axo::beginFlash(String filename) {
    if (filename.length() > (property::FILENAME_MAX_LEN + 2)) {
        return Message::FILE_TOO_LONG;
    }

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


bool Axo::setMotorAngle(float angle) {
    bool motorL = m_motorL.moveToAngle(m_startingPosL - angle);
    bool motorR = m_motorR.moveToAngle(m_startingPosR + angle);
    return (motorL && motorR);
}


void Axo::centerMotors() {
    m_motorL.center();
    m_motorR.center();
}


void Axo::setMotorAngleWithSpeed(float startAngle, float endAngle, int offset) {
    m_motorL.gotoAngleFixedSpeed(m_startingPosL - startAngle, m_startingPosL - endAngle, motor::MIDDLE_POINT + offset);
    m_motorR.gotoAngleFixedSpeed(m_startingPosR + startAngle, m_startingPosR + endAngle, motor::MIDDLE_POINT - offset);
}


void Axo::writeMotors(int offset) {
    m_motorL.writeMicroseconds(motor::MIDDLE_POINT + offset);
    // m_motorR.writeMicroseconds(motor::MIDDLE_POINT - offset);
}


bool Axo::addTimeToBuf(unsigned long t) {
    // time comes in microseconds, convert to millis * 10 and use 8 bit.
    uint8_t timeDif{};
    if (t/100 > 255) {
        timeDif = 255;
    } else {
        timeDif = static_cast<uint8_t>(t/100);
    }
    bool flashHasSpace = addCharToBuf(timeDif);
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

    bool p = saveQuat(quatProp);
    bool a = saveQuat(quatAda);

    return p && a;
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
    unsigned long fileSizeNeeded = (numDataPoints * property::QUAT_DATA_SIZE * numQuats);
    if (fileSizeNeeded > property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE)
        fileSizeNeeded = property::FLASH_MAX_PAGES * property::FLASH_PAGE_SIZE;
    return fileSizeNeeded;
}
