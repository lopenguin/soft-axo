/*
*   PID library meant for use with 50 Hz PWM output.
*   Made for the Amber Lab at Caltech – by K. Pochana and N. Janwani
*/

#ifndef axPID
#define axPID

#include "axStack.h"

class axPID {
    private:
        const int _N;           // number of samples to average (derivative)
        const int _fill;        // initial fill value for the derivative
        const float _kP;        // proportional constant
        const float _kD;        // derivative constant
        const float _deadband;  // deadband of pwm where no motor movement occurs (amplitude)
        const float _max;       // max allotted amplitude of pwm from _zero
        const float _zero;      // _zero pwm (no motor movement)

        float _timer;           // timer variable
        axStack _derivs;        // error averaging object
    
    public:
        typedef enum {
            P,
            PD,
            PID
        } control_t;

        axPID();

        bool initialize();

        float proportional(float curr, float des);

        float derivative(float dE);

        float format(float value);

        int getPWM(float curr, float des);
};

#endif