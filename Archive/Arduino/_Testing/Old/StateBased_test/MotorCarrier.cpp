#include "MotorCarrier.h"
#include "constants.h"

void MotorCarrier::begin() {
    pinMode(pin::MOTOR_1, OUTPUT);
    pinMode(pin::MOTOR_2, OUTPUT);
    pinMode(pin::POT_1, INPUT);
    pinMode(pin::POT_2, INPUT);
}
