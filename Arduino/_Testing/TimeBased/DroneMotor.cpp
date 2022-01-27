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


void DroneMotor::readPot() {
    m_lastPos = analogRead(m_potPin);
}


bool DroneMotor::moveToAngle(float angle) {
    int currentPos{ analogRead(m_potPin) };

    // safety stop
    if (currentPos > 900 || currentPos < 200) {
        Serial.println(currentPos);
        center();
        return true;
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
