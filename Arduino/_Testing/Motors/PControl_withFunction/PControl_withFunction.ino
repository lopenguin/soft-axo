/*
Ports Neil's PControl code to a function for incorporation in Axo class.

Lorenzo Shaikewitz, 1/20/2022

NOT TESTED!!
*/
#include <Servo.h>
#include "constants.h"


class DroneMotor : public Servo {
public:
    DroneMotor(int motorPin, int potPin) : Servo(), m_motorPin{motorPin}, m_potPin{potPin}
    {/*does nothing*/}

    void attach() {
        Servo::attach(m_motorPin, motor::PWM_LOW, motor::PWM_HIGH);
        pinMode(m_potPin, INPUT);

        // make sure we know position
        m_lastPos = analogRead(m_potPin);
    }

    // call to move motor to the center point (stop)
    void center() {
        writeMicroseconds(motor::MIDDLE_POINT);

        // make sure we know position
        m_lastPos = analogRead(m_potPin);
    }

    /*Call repeatedly (100 Hz) to move the motor to this absolute position.
      Angle must be between 0 and 343 degrees (can be float)
      Returns T if position reached, F if needs to be called again.
    */
    bool moveToAngle(float angle) {
        int currentPos{ analogRead(m_potPin) };
        int desiredPos = angle / motor::POT_TO_DEG;

        if (!isNear(currentPos, desiredPos) || !isNear(m_lastPos, currentPos, 5)) {
            // save lastPos for isNear next time
            m_lastPos = currentPos;

            int pwm = p(currentPos, desiredPos);
            writeMicroseconds(pwm);

            // not there yet
            return false;
        } else {
            // we've arrived at our position!
            return true;
        }
    }

private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastPos{0};

    int p(int curr, int desired) {
        int pwmVal = 0.2 * (curr - desired) + 1500;
        if (pwmVal < 1550 && pwmVal > 1510) {
            pwmVal = 1550;
        } else if (pwmVal > 1470 && pwmVal < 1490) {
            pwmVal = 1470;
        }
        if (pwmVal > motor::MAX_FORWARD_SPEED) {
            pwmVal = motor::MAX_FORWARD_SPEED;
        } else if (pwmVal < motor::MAX_BACKWARD_SPEED) {
            pwmVal = motor::MAX_BACKWARD_SPEED;
        }
        return pwmVal;
    }

    bool isNear(int x1, int x2, int range = motor::UNCERTAINTY) {
        if (x1 > x2 + range || x1 < x2 - range) {
            return false;
        }
        return true;
    }
};


DroneMotor motorL(pin::MOTOR_L, pin::POT_L);
DroneMotor motorR(pin::MOTOR_R, pin::POT_R);


void setup() {
    Serial.begin(9600);
    Serial.setTimeout(20);

    // motor startup sequence
    motorL.attach();
    motorR.attach();
    delay(motor::CALIB_DELAY);
    motorL.center();
    motorR.center();
    delay(500);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    Serial.println("Enter an angle between 0 and 343 degrees.");
    while (Serial.available() == 0) {
        motorR.center();
        motorL.center();
    }
  int desired = Serial.parseInt();
  Serial.read();
  if (desired >= 0 && desired <= 343) {
    Serial.print("Entered ");
    Serial.print(desired);
    Serial.println(" --> centering...");

    // loops while we have not hit the goal OR the velocity is not 0 (for both motors)
    while (1) {
        if (motorL.moveToAngle(desired) && motorR.moveToAngle(desired)) {
            break;
        }

        // delay to emulate 100 Hz
        delay(10);
    }
  } else {
    Serial.println("Invalid input, please try again.");
  }
  Serial.println("Done!");
  Serial.println("");
}
