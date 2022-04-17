/*
Tests reading the motor potentiometers. No libraries needed. Should work with
Serial Plotter as well.

Lorenzo Shaikewitz, 4/17/2022
*/
#include <Metro.h>

// use Serial for printing to the monitor and Serial1 for wireless with Wixel.
#define SerialOut Serial
// #define SerialOut Serial1

// Constants
const short POTL_PIN{15};
const short POTR_PIN{14};
const int SAMPLERATE{100};
Metro potTimer = Metro(SAMPLERATE); // read the FSR at SAMPLERATE Hz

void setup() {
    pinMode(POTL_PIN, INPUT);
    pinMode(POTR_PIN, INPUT);
}

void loop() {
    if (potTimer.check()) {
        SerialOut.printf("%d, %d\n", analogRead(POTL_PIN), analogRead(POTR_PIN));
    }
}
