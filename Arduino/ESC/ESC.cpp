/*
    ESC.h - Library for controlling BLDC ESCs with general microcontrollers
    Created by the Amber Lab at Caltech for an ankle-exoskeleton
*/

#include "Arduino.h"
#include "ESC.h"

void ESC::pulsewidth(int dutytime)
{
    digitalWrite(_pin, HIGH);
    delayMicroseconds(dutytime);
    digitalWrite(_pin, LOW);
}

ESC::ESC(const int pin, const int offtime, const int calibration_time, const int calibration_pwm)
{
    _pin = pin;
    _calibration_time = calibration_time;
    _calibration_pwm = calibration_pwm;
    _offtime = offtime;
    _timer = 0;
}

void ESC::turn(int dutytime)
{
    if (_timer == 0) {
        _timer = millis();
    }
    if (millis() - _timer >= _offtime)
    {
        pulsewidth(dutytime);
        _timer = 0;
    }
}

void ESC::calib() {
    pinMode(_pin, OUTPUT);
    Serial.begin(9600);
    Serial.println("Calibrating...");
    double curr = millis();
    while (millis() - curr < _calibration_time)
    {
        turn(_calibration_pwm);
    }
}
