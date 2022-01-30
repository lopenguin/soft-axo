/*
Lorenzo Shaikewitz, 1/21/2022
*/
#include "DroneMotor.h"

#include "constants.h"
#include <Arduino.h>
#include <PWMServo_Lorenzo.h>

void DroneMotor::attach() {
    PWMServo::attach(m_motorPin, motor::PWM_LOW, motor::PWM_HIGH);
    pinMode(m_potPin, INPUT);

    // make sure we know position
    m_lastPos = analogRead(m_potPin);
}


void DroneMotor::center() {
    writeMicroseconds(motor::MIDDLE_POINT);

    // make sure we know position
    m_lastPos = analogRead(m_potPin);
}


int DroneMotor::readPot() {
    m_lastPos = analogRead(m_potPin);
    return m_lastPos;
}


bool DroneMotor::gotoAngleFixedSpeed(float startAngle, float endAngle, int speed) {
    int currentPos{ analogRead(m_potPin) };
    int desiredPos = endAngle / motor::POT_TO_DEG;


    // safety stop
    if (currentPos > 924) {
        Serial.println("In 'safety stop' position");
        writeMicroseconds(motor::MIN_SPEED_ABOVE); // proceed slowly
        return false;
    }
    if (currentPos < 100) {
        Serial.println("In 'safety stop' position");
        writeMicroseconds(motor::MIN_SPEED_BELOW); // proceed slowly
        return false;
    }


    bool forwards{ endAngle > startAngle };
    // stop if passed position
    if (forwards) {
        if (currentPos > endAngle) {
            writeMicroseconds(motor::MIN_SPEED_ABOVE); // proceed slowly
            return false;
        }
        if (currentPos < startAngle) {
            writeMicroseconds(motor::MIN_SPEED_ABOVE); // proceed slowly
            return false;
        }
    } else {
        if (currentPos < endAngle) {
            writeMicroseconds(motor::MIN_SPEED_BELOW); // proceed slowly
            return false;
        }
        if (currentPos > startAngle) {
            writeMicroseconds(motor::MIN_SPEED_BELOW); // proceed slowly
            return false;
        }
    }

    writeMicroseconds(speed);
    return true;
}


bool DroneMotor::moveToAngle(float angle) {
    int currentPos{ analogRead(m_potPin) };

    // safety stop
    if (currentPos > 924) {
        Serial.println("In 'safety stop' position");
        writeMicroseconds(motor::MIN_SPEED_ABOVE); // proceed slowly
        return false;
    }
    if (currentPos < 100) {
        Serial.println("In 'safety stop' position");
        writeMicroseconds(motor::MIN_SPEED_BELOW); // proceed slowly
        return false;
    }

    int desiredPos = angle / motor::POT_TO_DEG;

    const auto isNear {
        [](int x1, int x2, int range = motor::UNCERTAINTY) {
            return !(x1 > (x2 + range) || x1 < (x2 - range));
        }
    };

    if (!isNear(currentPos, desiredPos) || !isNear(m_lastPos, currentPos, 5)) {
        // save lastPos for isNear next time
        m_lastPos = currentPos;
        int currentPos{ analogRead(m_potPin) };
        int desiredPos = angle / motor::POT_TO_DEG;

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
    if (pwmVal < motor::MIN_SPEED_ABOVE && pwmVal > 1510) {
        pwmVal = motor::MIN_SPEED_ABOVE;
    } else if (pwmVal > motor::MIN_SPEED_BELOW && pwmVal < 1490) {
        pwmVal = motor::MIN_SPEED_BELOW;
    }
    if (pwmVal > motor::MAX_FORWARD_SPEED) {
        pwmVal = motor::MAX_FORWARD_SPEED;
    } else if (pwmVal < motor::MAX_BACKWARD_SPEED) {
        pwmVal = motor::MAX_BACKWARD_SPEED;
    }
    return pwmVal;
}
