/*
  Code to test the ESC powering a motor. Uses servo class.

  Neil added euler angle input to p control to determine angle! Works with two
  motors.
  Lorenzo Shaikewitz, 1/17/2022
*/
#include <PWMServo_Lorenzo.h>

namespace pin {
const short MOTOR_R{3};
const short MOTOR_L{4};

const short POT_L{A7};
const short POT_R{A8};
}

const int MIDDLE_POINT = 1500;
const int PWM_LOW = 1000;
const int PWM_HIGH = 2000;
const int MAX_FORWARDS_SPEED = 1560;
const int MAX_BACKWARDS_SPEED = 1440;
const int CALIB_DELAY = 2000;
const int UNCERTAINTY = 10; // how close to get to goal (+/-)

const int MIDDLE{1522};
const int stepTime{1000};
const int maxSpeed1{50};
unsigned long stepStartTime{0};

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
    void writeToAngle(int angle, int speed, unsigned long t) {
        int currentPos = analogRead(m_potPin);
        int desiredPos = speed / 343.0 * 1024;

        if (((currentPos + 50) % 1024 > desiredPos) &&
            ((currentPos - 50) % 1024 < desiredPos)) {
            stop();
            return;
        }
        write(speed, t);
    }


private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastWriteVal;
    unsigned long m_lastWriteTime{};


    const int MIDDLE_HIGH{1520};
    const int MIDDLE_LOW{1500};
    const uint MAX_WRITE_TIME{200};
};


DroneMotor motor_L(pin::MOTOR_L, pin::POT_L);
DroneMotor motor_R(pin::MOTOR_R, pin::POT_R);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(20);
  motor_L.attach();
  motor_R.attach();
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
    motor_L.stop();
    motor_R.stop();
  }
  int desired = Serial.parseInt();
  Serial.read();
  if (desired >= 0 && desired <= 343) {
    Serial.print("Entered ");
    Serial.print(desired);
    Serial.println(" --> centering...");
    int pL = (int)analogRead(pin::POT_L);
    int last_pL = pL;
    int pR = (int)analogRead(pin::POT_R);
    int last_pR = pR;

    int goal = (int)(desired / 343.0 * 1023);
    unsigned long startTime = millis();

    // loops while we have not hit the goal OR the velocity is not 0 (for both motors)
    while (1) {
        bool leftGood{false};
        if (!isNear(pL / 1023.0 * 343, desired, UNCERTAINTY) || !isNear(last_pL, pL, 5)) {
            last_pL = pL;
            pL = (int)analogRead(pin::POT_L);
            goal = (int)((270-desired) / 343.0 * 1023);
            int pwmL = p(pL, goal);
            motor_L.write(pwmL, millis());

            int offset = pwmL - MIDDLE;
            // motor_R.write(MIDDLE - offset, millis());
        } else {
            leftGood = true;
        }

        bool rightGood{false};
        if (!isNear(pR / 1023.0 * 343, desired, UNCERTAINTY) || !isNear(last_pR, pR, 5)) {
            last_pR = pR;
            pR = (int)analogRead(pin::POT_R);
            goal = (int)((90+desired) / 343.0 * 1023);
            int pwmR = p(pR, goal);
            motor_R.write(pwmR, millis());
        } else {
            rightGood = true;
        }

        Serial.print(pL / 1023.0 * 343);
        Serial.print(" ");
        Serial.print(desired);
        Serial.print('\t');

        Serial.print(pR / 1023.0 * 343);
        Serial.print(" ");
        Serial.println(desired);

        if (leftGood && rightGood) {
            break;
        }

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
