/*

Lorenzo Shaikewitz, 8/10/2021
*/
#include "Axo.h"
#include "constants.h"

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

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


}


bool Axo::saveFromBuf(char* buf, const int bufSize = property::FLASH_PAGE_SIZE) {
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


int Axo::timeToFileSize(int runTimeSeconds) {
    int numDataPoints = runTimeSeconds * property::IMU_UPDATE_HZ;
    int pagesNeeded = (numDataPoints * property::QUAT_DATA_SIZE) / property::FLASH_PAGE_SIZE;
    if (pagesNeeded > property::FLASH_MAX_PAGES)
        pagesNeeded = property::FLASH_MAX_PAGES;
    return pagesNeeded;
}
