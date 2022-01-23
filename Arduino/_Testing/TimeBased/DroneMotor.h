/*
Holds the DroneMotor servo class extension for controlling ankle exo motors.

Lorenzo Shaikewitz, 1/21/2022
*/
#ifndef DRONE_MOTOR_H
#define DRONE_MOTOR_H


#include <Servo.h>
#include "constants.h"


class DroneMotor : public Servo {
public:
    DroneMotor(int motorPin, int potPin) : Servo(), m_motorPin{motorPin}, m_potPin{potPin}
    {/*does nothing*/}

    void attach();

    // call to move motor to the center point (stop)
    void center();

    /*Call repeatedly (100 Hz) to move the motor to this absolute position.
      Angle must be between 0 and 343 degrees (can be float)
      Returns T if position reached, F if needs to be called again.
    */
    bool moveToAngle(float angle);

private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastPos{0};

    int p(int curr, int desired);
};


#endif
