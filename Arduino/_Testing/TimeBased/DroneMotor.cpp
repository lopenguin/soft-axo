/*
Lorenzo Shaikewitz, 1/21/2022
*/
#include "DroneMotor.h"

#include "constants.h"
#include <Arduino.h>
#include <Servo.h>

void DroneMotor::attach() {
    Servo::attach(m_motorPin, motor::PWM_LOW, motor::PWM_HIGH);
    pinMode(m_potPin, INPUT);

    // make sure we know position
    m_lastPos = analogRead(m_potPin);
}


void DroneMotor::center() {
    writeMicroseconds(motor::MIDDLE_POINT);

    // make sure we know position
    m_lastPos = analogRead(m_potPin);
}


bool DroneMotor::moveToAngle(float angle) {
    int currentPos{ analogRead(m_potPin) };
    int desiredPos = angle / motor::POT_TO_DEG;

    const auto isNear {
        [](int x1, int x2, int range = motor::UNCERTAINTY) {
            return !(x1 > (x2 + range) || x1 < (x2 - range));
        }
    }

    if (!isNear(currentPos, desiredPos) || !isNear(m_lastPos, currentPos, 5)) {
        // save lastPos for isNear next time
        m_lastPos = currentPos;

        int pwm = p(currentPos, desiredPos);
        writeMicroseconds(pwm);

        // not there yet
        return false;
    } else {
        // we've arrived at our position!
        return true;
    }
}


int DroneMotor::p(int curr, int desired) {
    int pwmVal = 0.2 * (curr - desired) + 1500;
    if (pwmVal < 1550 && pwmVal > 1510) {
        pwmVal = 1550;
    } else if (pwmVal > 1470 && pwmVal < 1490) {
        pwmVal = 1470;
    }
    if (pwmVal > motor::MAX_FORWARD_SPEED) {
        pwmVal = motor::MAX_FORWARD_SPEED;
    } else if (pwmVal < motor::MAX_BACKWARD_SPEED) {
        pwmVal = motor::MAX_BACKWARD_SPEED;
    }
    return pwmVal;
}
