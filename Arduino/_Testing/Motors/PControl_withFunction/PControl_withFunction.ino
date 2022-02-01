/*
Ports Neil's PControl code to a function for incorporation in Axo class.

Lorenzo Shaikewitz, 1/20/2022

NOT TESTED
*/
#include <PWMServo.h>
#include "constants.h"

class DroneMotor : public PWMServo {
public:
    DroneMotor(int motorPin, int potPin) : PWMServo(),
                    m_motorPin{motorPin}, m_potPin{potPin},
                    m_lastWriteVal{1500}
    {/*does nothing*/}


    // connects motor to a pin
    void attach() {
        PWMServo::attach(m_motorPin);
        pinMode(m_potPin, INPUT);

    }

    // stops motor from moving
    void stop(int lastWriteVal = MIDDLE) {
        if (m_lastWriteVal > MIDDLE) {
            writeMicroseconds(MIDDLE_LOW);
        } else {
            writeMicroseconds(MIDDLE_HIGH);
        }
        m_lastWriteVal = lastWriteVal;
    }

    // writes the motor to the given speed (with safety stop)
    void write(int speed, unsigned long t) {
        // safety stop
        if (speed == m_lastWriteVal) {
            if ((t - m_lastWriteTime) > MAX_WRITE_TIME) {
                stop(speed);
            }
            return;
        }
        m_lastWriteTime = t;
        m_lastWriteVal = speed;


        writeMicroseconds(speed);
    }

    // stops the motor when in the neighborhood of a desired angle
    // void writeToAngle(int angle, int speed, unsigned long t) {
    //     int currentPos = analogRead(m_potPin);
    //     int desiredPos = speed / 343.0 * 1024;
    //
    //     if (((currentPos + 50) % 1024 > desiredPos) &&
    //         ((currentPos - 50) % 1024 < desiredPos)) {
    //         stop();
    //         return;
    //     }
    //     write(speed, t);
    // }


    bool writeToAnglePControl(float angle, unsigned long t) {
        int currentPos{ analogRead(m_potPin) };
        int desiredPos = angle / motor::POT_TO_DEG;

        if (!isNear(currentPos, desiredPos) || !isNear(m_lastPos, currentPos, 5)) {
            m_lastPos = currentPos;
            int currentPos{ analogRead(m_potPin) };
            int desiredPos = angle / motor::POT_TO_DEG;

            int pwm = p(currentPos, desiredPos);
            Serial.print(pwm);
            write(pwm, t);

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


    int readPot() {
        m_lastPos = analogRead(m_potPin);
        return m_lastPos;
    }


private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastWriteVal;
    unsigned long m_lastWriteTime{};

    const int MIDDLE{1522};
    const int MIDDLE_HIGH{1520};
    const int MIDDLE_LOW{1500};
    const uint MAX_WRITE_TIME{200};

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
        unsigned long currentTime = millis();
        bool lThere{motorL.writeToAnglePControl(startingPosL, currentTime)};
        bool rThere{motorR.writeToAnglePControl(startingPosR, currentTime)};

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
    unsigned long startTime = millis();

    // loops while we have not hit the goal OR the velocity is not 0 (for both motors)
    while (1) {
        unsigned long currentTime{millis()};
        bool lThere{motorL.writeToAnglePControl(startingPosL - desired, currentTime)};
        bool rThere{motorR.writeToAnglePControl(startingPosR + desired, currentTime)};

        if (lThere && rThere) {
            Serial.print('\n');
            break;
        }
        Serial.print('\n');

        if (millis() - startTime > 2000) {
            break;
        }
    }
  } else {
    Serial.println("Invalid input, please try again.");
  }
  Serial.println("Done!");
  Serial.println("");
}
