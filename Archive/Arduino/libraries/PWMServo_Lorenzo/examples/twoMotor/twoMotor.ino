// Sweep
// by BARRAGAN <http://barraganstudio.com>

// http://arduiniana.org/libraries/pwmservo/

//   Board                     SERVO_PIN_A   SERVO_PIN_B   SERVO_PIN_C
//   -----                     -----------   -----------   -----------
//   Arduino Uno, Duemilanove       9            10          (none)
//   Arduino Mega                  11            12            13
//   Sanguino                      13            12          (none)
//   Teensy 1.0                    17            18            15
//   Teensy 2.0                    14            15             4
//   Teensy++ 1.0 or 2.0           25            26            27
//   Teensy LC & 3.x                 (all PWM pins are usable)

#include "PWMServo_Lorenzo.h"

PWMServo myservo1;  // create servo object to control a servo
PWMServo myservo2;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position

void setup() {
//  myservo.attach(SERVO_PIN_A);  // attaches the servo on pin 9 to the servo object
  myservo1.attach(3, 1000, 2000); // some motors need min/max setting
  myservo2.attach(4, 1000, 2000); // some motors need min/max setting
  delay(500);
  myservo1.writeMicroseconds(1500);
  myservo2.writeMicroseconds(1500);
  delay(500);
}


void loop() {
  myservo1.writeMicroseconds(1500);
  myservo2.writeMicroseconds(1500);
  delay(1000);
  myservo1.writeMicroseconds(1600);
  myservo2.writeMicroseconds(1600);
  delay(2000);
}
