/*
  Code to test the ESC powering a motor. Uses servo class.
  Lorenzo Shaikewitz, 1/17/2022
*/
#include <Servo.h>

namespace pin {
const short MOTOR_R{3};
const short MOTOR_L{4};
}

const int MIDDLE_POINT = 1500;
const int PWM_LOW = 1000;
const int PWM_HIGH = 2000;
const int MAX_FORWARDS_SPEED = 1560;
const int MAX_BACKWARDS_SPEED = 1440;
const int CALIB_DELAY = 7000;
const int UNCERTAINTY = 10; // how close to get to goal (+/-)

Servo motor_L;
Servo motor_R;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(20);
  motor_L.attach(pin::MOTOR_L, PWM_LOW, PWM_HIGH);
  motor_R.attach(pin::MOTOR_R, PWM_LOW, PWM_HIGH);
  delay(CALIB_DELAY);
  motor_L.writeMicroseconds(MIDDLE_POINT);
  motor_R.writeMicroseconds(MIDDLE_POINT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void loop() {
  Serial.println("Enter an angle between 0 and 343 degrees.");
  while (Serial.available() == 0) {
    motor_L.writeMicroseconds(MIDDLE_POINT);
    motor_R.writeMicroseconds(MIDDLE_POINT);
  }
  int desired = Serial.parseInt();
  if (desired >= 0 && desired <= 343) {
    Serial.print("Entered ");
    Serial.print(desired);
    Serial.println(" --> centering...");
    int pL = (int)analogRead(A7);
    int last_pL = pL;
    int goal = (int)(desired / 343.0 * 1023);
    
    // loops while we have not hit the goal OR the velocity is not 0
    while (!isNear(pL / 1023.0 * 343, desired, UNCERTAINTY) || !isNear(last_pL, pL, 5)) {
      last_pL = pL;
      pL = (int)analogRead(A7);
      goal = (int)(desired / 343.0 * 1023);
      int pwmL = p(pL, goal);
      motor_L.writeMicroseconds(pwmL);
      
      Serial.print(pL / 1023.0 * 343);
      Serial.print(" ");
      Serial.println(desired);
      delay(10); // we need this delay to measure velocity accurately -> ~100 Hz controller
    }
  } else {
    Serial.println("Invalid input, please try again.");
  }
  Serial.println("Done!");
  Serial.println("");
}

int p(int curr, int desired) {
  int pwmVal = 0.2 * (curr - desired) + 1500;
  if (pwmVal < 1550 && pwmVal > 1510) {
    pwmVal = 1550;
  } else if (pwmVal > 1470 && pwmVal < 1490) {
    pwmVal = 1470;
  }
  if (pwmVal > MAX_FORWARDS_SPEED) {
    pwmVal = MAX_FORWARDS_SPEED;
  } else if (pwmVal < MAX_BACKWARDS_SPEED) {
    pwmVal = MAX_BACKWARDS_SPEED;
  }
  return pwmVal;
}

boolean isNear(int x1, int x2, int range) {
  if (x1 > x2 + range || x1 < x2 - range) {
    return false;
  }
  return true;
}
