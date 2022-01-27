/*
Ports Neil's PControl code to a function for incorporation in Axo class.

Lorenzo Shaikewitz, 1/20/2022

NOT TESTED
*/
#include <Servo.h>
#include "constants.h"


class DroneMotor : public Servo {
public:
    DroneMotor(int motorPin, int potPin) : Servo(), m_motorPin{motorPin}, m_potPin{potPin}, m_print{ false }
    {/*does nothing*/}

    void enablePrinting() { m_print = true; }

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

    void readPot() {
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
            m_lastPos = currentPos;
            int currentPos{ analogRead(m_potPin) };
            int desiredPos = angle / motor::POT_TO_DEG;

            int pwm = p(currentPos, desiredPos);
            Serial.print(pwm);
            writeMicroseconds(pwm);

            if (m_print) {
                Serial.print(": ");
                Serial.print(desiredPos);
                Serial.print(", ");
                Serial.print(currentPos);
                Serial.print('\t');
            }
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
    bool m_print;

    int m_lastPos{0};

    int p(int curr, int desired) {
        int pwmVal = 0.5 * (curr - desired) + 1500;
        if (pwmVal < motor::MIN_SPEED_ABOVE && pwmVal > 1510) {
            pwmVal = motor::MIN_SPEED_ABOVE;
        } else if (pwmVal > motor::MIN_SPEED_BELOW && pwmVal < 1490) {
            pwmVal = motor::MIN_SPEED_BELOW;
        }
        if (pwmVal > motor::MAX_FORWARD_SPEED) {
            pwmVal = motor::MAX_FORWARD_SPEED;
        } else if (pwmVal < motor::MAX_BACKWARD_SPEED) {
            pwmVal = motor::MAX_BACKWARD_SPEED;
        }

        // temp to remove spin in 1 direction
        // if (pwmVal < 1500) {
        //     pwmVal = 1500;
        // }
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

int startingPosL{270};
int startingPosR{90};

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

    motorL.enablePrinting();
    motorR.enablePrinting();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // initialize positions
    Serial.print("Starting at 90 degrees ");
    Serial.println(" --> centering...");

    motorL.readPot();
    motorR.readPot();

    // loops while we have not hit the goal OR the velocity is not 0 (for both motors)
    while (1) {
        bool lThere{motorL.moveToAngle(startingPosL)};
        bool rThere{motorR.moveToAngle(startingPosR)};

        if (lThere && rThere) {
            Serial.print('\n');
            break;
        }
        Serial.print('\n');

        // delay to emulate 100 Hz
        delay(10);
    }
}

void loop() {
    Serial.println("Enter an angle between 0 and 180 degrees.");
    while (Serial.available() == 0) {
        motorR.center();
        motorL.center();
    }
  int desired = Serial.parseInt();
  Serial.read();
  if (desired >= 0 && desired <= 180) {
    Serial.print("Entered ");
    Serial.print(desired);
    Serial.println(" --> centering...");
    // int desiredR = desired + 180 % 360;
    motorL.readPot();
    motorR.readPot();

    // loops while we have not hit the goal OR the velocity is not 0 (for both motors)
    while (1) {
        bool lThere{motorL.moveToAngle(startingPosL - desired)};
        bool rThere{motorR.moveToAngle(startingPosR + desired)};

        if (lThere && rThere) {
            Serial.print('\n');
            break;
        }
        Serial.print('\n');

        // delay to emulate 100 Hz
        delay(1);
    }
  } else {
    Serial.println("Invalid input, please try again.");
  }
  Serial.println("Done!");
  Serial.println("");
}
