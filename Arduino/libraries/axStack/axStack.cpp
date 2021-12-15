/*
    axStack.h - computes rolling average for simple discrete-based derivative calculations.
    Created by the Amber Lab at Caltech for Axo.
*/

#include "Arduino.h"
#include "axStack.h"

axStack::axStack() {
    // do nothing
}

axStack::initialize(int size, double fillvalue)
{
    if (size < 0) {
        return false;
    }
    _size = size;
    _members = new double[_size];
    for (int i = 0; i < _size; i++) {
        _members[i] = fillvalue;
    }
    _avg = fillvalue;
    _position = 0;
    return true;
}

void axStack::enqueue(double member) {
    double old = _members[_position];
    _avg = _avg + (member - old) / _size;
    _members[_position] = member;
    _position++;
    if (_position >= _size) {
        _position = 0;
    }
}

void axStack::flush(double fillvalue) {
    for (int i = 0; i < _size; i++) {
        _members[i] = fillvalue;
    }
}

double axStack::get_average() {
    return _avg;
}