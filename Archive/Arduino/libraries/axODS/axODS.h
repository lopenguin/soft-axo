/*
    axODS.h - Arduino C++ library for the 'oriented dual servo' configuration for two HSAs using a variation of PD-control.
    Created by the Amber Lab at Caltech for Axo.
*/

#ifndef axODS
#define axODS

#include "Arduino.h"
#include "axStack.h"
#include "Servo.h"

class axODS
{
private:
    // Pin definitions
    const int _POT_R;       // right potentiometer.
    const int _POT_L;       // left potentiometer.
    const int _HSA_R;       // right HSA ESC PWM pin.
    const int _HSA_L;       // left HSA ESC PWM pin.

    // Constants
    const float _kP;        // proportional constant.
    const float _kD;        // derivative constant.
    const int _PWM_0;       // 'rest' position for HSA ESC.
    const int _PWM_BAND;    // height of deadband PWM where no motor movement occurs.
    const int _PWM_MAX;     // maximum alloted PWM from _PWM_0 (amplitude).
    const int _PWM_CALIB;   // time in milliseconds required for calibration.
    const int _N = 10;      // samples to average.
    const int _FILL;        // value to start with (will be setup during calibration).

    // Variables
    int _pot_R;             // current right HSA position.
    int _pot_L;             // current left HSA position.
    float timer;            // timer for derivative purposes.
    bool initialized;       // checks if initialization has been run.
    bool calibrated;        // checks if HSAs have been calibrated.

    // Objects
    axStack leftDeriv;      // derivatives for the left HSA.
    axStack rightDeriv;     // derivatives for the right HSA.
    Servo _leftHSA;         // left HSA Servo object
    Servo _rightHSA         // right HSA Servo object

    

public:
    axODS();

    /*
    * Initializes axODS with proper pins. Should only be called once.
    * See definitions above for argument definitions.
    * 
    * Returns: true if successful.
    *          false if unsuccessful.
    */
    bool initialize(int POT_R,
                    int POT_L,
                    int HSA_R,
                    int HSA_L,
                    int PWM_0,
                    int PWM_BAND,
                    int PWM_MAX,
                    int PWM_CALIB);

    /*
    * Returns the position of the right HSA in radians.
    */
    int getRightPos();

    /*
    * Returns the position of the left HSA in radians.
    */
    int getLeftPos();

    /*
    * Must be run AFTER initialize()
    * When run the first time, the ESC will calibrate (low then high beep).
    * When run subsequent times, the motors will just stop for PWM_CALIB milliseconds.
    * kP is the proportional constant.
    * kD is the derivative constant.
    * Returns: true if calibration was successful.
    *          false if not.
    */
    bool calibrate(float kP, float kD);

    /*
    * Attempts to center the right HSA to a certain position in radians.
    * Returns: true if a motor command is sent
    *          false if no command could be sent
    */
    bool centerRight(float position);

    /*
    * Attempts to center the left HSA to a certain position in radians.
    * Returns: true if a motor command is sent
    *          false if no command could be sent
    */
    bool centerLeft(float position);

    /*
    * Allows both motors controlling the HSAs to spin freely.
    * To be used if something goes wrong, manual HSA manipulation is needed, etc...
    * Returns: true if a motor command is sent.
    *          false if no command could be sent.
    */
    bool relax();
};

#endif