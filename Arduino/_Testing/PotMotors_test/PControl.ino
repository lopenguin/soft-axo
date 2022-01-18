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
}

void loop() {
  Serial.flush();
  int mL = (int)analogRead(A7);
  int desired = curve(millis());
  int pwmL = 0.2 * (mL - desired) + 1500;
  if (pwmL < 1550 && pwmL > 1510) {
    pwmL = 1550;
  } else if (pwmL > 1480 && pwmL < 1490) {
    pwmL = 1480;
  }
//  Serial.print(mL);
//  Serial.print("\t");
//  Serial.print(desired);
//  Serial.print("\t");
  Serial.println(desired);
  motor_L.writeMicroseconds(pwmL);
}

int curve(float t) {
  int factor = pow(10, 3);
  return 500 + 250 * sin(t / factor);
}
