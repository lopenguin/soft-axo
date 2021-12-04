#include "ESC.h"

int pulsewidth = 99999; // set large pulsewidth since inital Serial.parseInt() is always 0

double count = 0;

const int MIDDLE_POINT = 1500;        // Calibration point (it can be between like 1490-1520 or something)
const int PULSEWIDTH_LOW = 0;      // max spin in one direction
const int PULSEWIDTH_HIGH = 200;     // max spin in other direction
const int CALIBRATION_DELAY = 10000;  // length of calibration sequence
const int PULSEWIDTH_OFFTIME = 20;    // the off-time in the PWM duty cycle (default for most ESCs is 20 millis)

// these constants can be change
const int BAUD_RATE = 9600;           // serial baud rate
const int MOTOR_TURN_TIME = 5000;     // how long you want the motor to turn

#define ESC_PIN_1 3
#define ESC_PIN_2 4

ESC motor1(ESC_PIN_1, PULSEWIDTH_OFFTIME, CALIBRATION_DELAY, MIDDLE_POINT); // create ESC object to control the motor
ESC motor2(ESC_PIN_2, PULSEWIDTH_OFFTIME, CALIBRATION_DELAY, MIDDLE_POINT); // create ESC object to control the motor


void setup() {
  Serial.begin(BAUD_RATE); // starts and establishes the baud rate of the Serial Monitor
  Serial.println("Calibrating...");
  motor1.calib();
  motor2.calib();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {
//  if (pulsewidth > PULSEWIDTH_LOW) {
//    Serial.println("Enter PWM signal value in [1000, 2000]");
//  }
//  while (Serial.available() == 0) {
//    // loop and do nothing, wait until something appears in the Serial Monitor (ex: user enters some number)
//  }
//  pulsewidth = Serial.parseInt();
//  if (pulsewidth > 0) {
//    Serial.print("Entered ");
//    Serial.print(pulsewidth);
//    Serial.println(" --> turning motor...");
//    float timer = millis();
//    while (millis() - timer < MOTOR_TURN_TIME) {
//      //Serial.println("turiningifiasjfd");
//      //motor1.turn(pulsewidth + MIDDLE_POINT); // this line must be repeated in some sort of loop (in order to repeat the motor duty cycle)
//      //motor2.turn(-1 * pulsewidth + MIDDLE_POINT);
//      motor1.turn(pulsewidth);
//      motor2.turn(pulsewidth);
//    }
//    Serial.println("");
//  }
  motor1.turn(1650);
}
