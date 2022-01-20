/*
Code to test the ESC powering a motor. Uses servo class.

Lorenzo Shaikewitz, 1/17/2022
*/
#include <Servo.h>

namespace pin {
    const short MOTOR_R{3};
    const short MOTOR_L{4};
}

Servo motor_L;
Servo motor_R;

void setup() {
    Serial.begin(9600);

    motor_L.attach(pin::MOTOR_L, 1000, 2000);
    motor_R.attach(pin::MOTOR_R, 1000, 2000);
    delay(7000);
    motor_L.writeMicroseconds(1500);
    motor_R.writeMicroseconds(1500);
    delay(500);

    int writeLen = 3;
    int writes[writeLen] = {1700, 1200, 1500};

    for (int i = 0; i < writeLen; ++i) {
        int t = writes[i];
        Serial.println(t);
        motor_R.writeMicroseconds(t);
        motor_L.writeMicroseconds(t);
        delay(1000);
    }
}

void loop() {

}
