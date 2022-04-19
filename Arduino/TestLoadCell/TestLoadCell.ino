/*
Tests reading the load cell. No libraries needed, just an analog read.

Lorenzo Shaikewitz, 4/17/2022
*/
#include <Metro.h>

// use Serial for printing to the monitor and Serial1 for wireless with Wixel.
#define SerialOut Serial
// #define SerialOut Serial1

// Constants
const short LOADCELL_PIN{17};
const int SAMPLERATE{100};
Metro loadCelltimer = Metro(SAMPLERATE); // read the FSR at SAMPLERATE Hz

void setup() {
    pinMode(LOADCELL_PIN, INPUT);
}

void loop() {
    if (loadCelltimer.check()) {
        SerialOut.println(analogRead(LOADCELL_PIN));
    }
}
