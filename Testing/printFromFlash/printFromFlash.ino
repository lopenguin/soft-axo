/*
Prints a file to the Serial monitor from flash!
NOT WORKING!!!

Lorenzo Shaikewitz, 8/4/2021
*/
#include <SerialFlash.h>
#include <SPI.h>

const int FLASH_CS_PIN{ 6 };
const char FILE_NAME[15]{"test.csv"};

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(50);
    }

    // SerialFlash setup
    if (!SerialFlash.begin(FLASH_CS_PIN)) {
        Serial.println("Unable to access SPI Flash chip");
        while (1);
    }

    if (!SerialFlash.exists(FILE_NAME)) {
        Serial.println("File not found!");
        while (1);
    }
    Serial.println(FILE_NAME);
    Serial.println("---------");

    SerialFlashFile file = SerialFlash.open(FILE_NAME);
    while (file.available()) {
        char buf[256]{};
        file.read(buf, 256);
        Serial.print(buf);
    }
    file.close();
    Serial.println("\n---------");

}

void loop() {/*does nada*/}
