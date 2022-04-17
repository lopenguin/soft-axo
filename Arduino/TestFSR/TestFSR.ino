/*
Tests reading the FSR through a voltage divider. No libraries needed, but
FSRs may need to be readjusted every once in a while. Should work with Serial
Plotter as well.

Lorenzo Shaikewitz, 4/17/2022
*/
#include <Metro.h>

// use Serial for printing to the monitor and Serial1 for wireless with Wixel.
#define SerialOut Serial
// #define SerialOut Serial1

// Constants
const short FSRPIN{16};
const int SAMPLERATE{100};
Metro FSRtimer = Metro(SAMPLERATE); // read the FSR at SAMPLERATE Hz

void setup() {
    // may work better without this
    pinMode(FSRPIN, INPUT);
}

void loop() {
    if (FSRtimer.check()) {
        SerialOut.println(analogRead(FSRPIN));
    }
}
