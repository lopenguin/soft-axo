/*
Holds the DroneMotor servo class extension for controlling ankle exo motors.

Lorenzo Shaikewitz, 1/21/2022
*/
#ifndef DRONE_MOTOR_H
#define DRONE_MOTOR_H


#include <PWMServo_Lorenzo.h>
#include "constants.h"


class DroneMotor : public PWMServo {
public:
    DroneMotor(int motorPin, int potPin) : PWMServo(), m_motorPin{motorPin}, m_potPin{potPin}
    {/*does nothing*/}

    void attach();

    int readPot();

    // call to move motor to the center point (stop)
    void center();

    /*Call repeatedly (100 Hz) to move the motor to this absolute position.
      Angle must be between 0 and 343 degrees (can be float)
      Returns T if position reached, F if needs to be called again.
    */
    bool moveToAngle(float angle);

    bool gotoAngleFixedSpeed(float startAngle, float endAngle, int speed);

private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastPos{0};

    int p(int curr, int desired);
};


#endif
