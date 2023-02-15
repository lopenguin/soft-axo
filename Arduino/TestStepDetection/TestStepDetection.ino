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
const int SAMPLERATE{2}; // ms
Metro FSRtimer = Metro(SAMPLERATE); // read the FSR every SAMPLERATE ms
constexpr int FSR_HIGH_THRESH{400};
constexpr int FSR_LOW_THRESH{100};

unsigned long startTime{};
unsigned long startTimeFSR{};
unsigned long stepTime{};
float stepTimeAccum{};
float stepAlpha{0.92};
bool fsrHigh{};

float fsrAccum{};
float fsrAlpha{0.1}; // converges in ~3/alpha iterations

float accumStepTime{1500.0};
float a{0.9};

void setup() {
    // may work better without this
    pinMode(FSRPIN, INPUT);

    startTime = millis();
    SerialOut.begin(57600);
}

void loop() {
    if (FSRtimer.check()) {
        unsigned long currentTime = millis();
        unsigned long dt = currentTime - startTime;
        unsigned long dtFSR = currentTime - startTimeFSR;

        // int fsr = analogRead(FSRPIN);
        // fsrAccum = fsr * fsrAlpha + (1.0 - fsrAlpha)*fsrAccum;
        // float stepped{0};
        // // SerialOut.printf("%f, %d\n", accum, fsr);
        // if (fsrAccum > FSR_THRESH) {
        //     // want to record only the first heel strike
        //     if (!fsrHigh) {
        //         fsrHigh = true;
        //         stepTime = dt;
        //         accumStepTime = stepTime*a + (1.0 - a)*accumStepTime;
        //         startTime = currentTime;
        //         // SerialOut.printf("Step! %u, %f\n", stepTime, accumStepTime);
        //         stepped = fsrAccum;
        //     }
        // } else {
        //     fsrHigh = false;
        // }
        
        // SerialOut.printf("fsr:%f, step:%f\n",fsrAccum, stepped);


        int fsr = analogRead(FSRPIN);
        fsrAccum = fsr * fsrAlpha + (1.0 - fsrAlpha)*fsrAccum;
        float stepped{0};

        if (fsrAccum > FSR_HIGH_THRESH) {
            // record only if rising from a low point
            if (!fsrHigh) {
                fsrHigh = true;

                stepTime = dt;
                accumStepTime = stepTime*a + (1.0 - a)*accumStepTime;
                startTime = currentTime;
                SerialOut.printf("Step! %u, %f\n", stepTime, accumStepTime);
                stepped = fsrAccum;
            }
        } else if (fsrAccum < FSR_LOW_THRESH) {
            fsrHigh = false;
        }
        // SerialOut.printf("fsr:%f, step:%f\n",fsrAccum, stepped);

    }

    
}
