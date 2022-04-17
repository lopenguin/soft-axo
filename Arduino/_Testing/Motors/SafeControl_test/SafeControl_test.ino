#include <Servo.h>
#include <axStack.h>

const int MOTOR_PIN = 4;

const int CALIBRATION_PWM = 1500;
const int CALIBRATION_TIME = 7000;
const int CONTROL_RATE = 100; // in HZ
const double C = 0.005;
const double E = 2.71828;
const double K_p = 0.7;


Servo motor;
axStack deriv;

int curr = 0;
int prev = 0;
int turns = 0;
int goal = 0;

float timer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  deriv.initialize(100, 0.0);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  motor.attach(MOTOR_PIN);
  delay(1000);
  motor.writeMicroseconds(CALIBRATION_PWM);
  delay(CALIBRATION_TIME);
  digitalWrite(LED_BUILTIN, HIGH);
  prev = (int)analogRead(A8);
  timer = millis();
  goal = (int)(random(0, 1023));
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - timer > 1000) {
    goal = (int)(random(0,1023));
    timer = millis();
  }
  motor.writeMicroseconds(p(goal));
  curr = (int)analogRead(A8);
  deriv.enqueue(curr);
  if (curr - prev < -500) {
    turns++;
  } else if (curr - prev > 500) {
    turns--;
  }

  float theta = (curr + turns * 1023) / 1023.0 * 2 * PI;
  Serial.print(goal);
  Serial.print(" ");
  //Serial.print(p(goal));
  //Serial.print(" ");
  Serial.println(get_pos());
  delay(1000.0 / CONTROL_RATE);
  prev = curr;
}

int p(int desired) {
  //  if (get_pos() >= 1000) {
  //    return 1500;
  //  } else if (curr <= 23) {
  //    return 1500;
  //  }
  int ret = 1500 + (desired - get_pos()) * K_p;
  ret += pow(E,1 / (C * get_pos())); 
  ret -= pow(E, 1 / (C * (1023 - get_pos())));
  return min(1600, max(1400, ret));
}

int get_pos() {
  int ret = curr + 1023 * turns;
  return max(0, min(1023, ret));
}
