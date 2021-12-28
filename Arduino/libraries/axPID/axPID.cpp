#include "axPID.h"

axPID::axPID() {
    _timer = 0;
    _N = 10;
    _kP = 0;
    _kD = 0;
}

bool axPID::initialize(float kP, float kD) {
    _kP = kP;
    _kD = kD;
}

float axPID::proportional(float curr, float des) {
    return des - curr;
}

float axPID::derivative(float dE) {
    _derivs.enqueue(dE);
    return _derivs.get_slope();
}

float axPID::format(float value) {
    value += _zero;
    if (value > _max + _zero) {
        return _max + _zero;
    } else if (value < _max - _zero) {
        return _max - _zero;
    } else if (value > _zero && value < _deadband + _zero) {
        return _zero;
    } else if (value < _zero && value > _zero - _deadband) {
        return _zero;
    }
    return value;
}

float axPID::getPWM(float curr, float des) {
    float prop = proportional(curr, des);
    float error = (des - curr) / (millis() - timer);
    timer = millis();
    float deriv = derivative(error);
    float value = _kP * prop + _kD * deriv;
    return format(value);
}