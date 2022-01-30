/*
Testing operation of motors with PWMServo

Lorenzo Shaikewitz, 1/29/2022
*/

#include <PWMServo_Lorenzo.h>

const short motorPinL{4};
const short motorPinR{3};
const short potPinL{21};
const short potPinR{22};

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
    const uint MAX_WRITE_TIME{500};
};

DroneMotor motorL(motorPinL, potPinL);
DroneMotor motorR(motorPinR, potPinR);

void setup() {
    motorL.attach();
    motorR.attach();
    delay(500);
    motorL.writeMicroseconds(1500);
    motorR.writeMicroseconds(1500);
    delay(500);
    stepStartTime = millis();
    while (1) {
        unsigned long currentTime = millis();
        motorL.writeToAngle(270, MIDDLE - maxSpeed1, currentTime);
        motorR.writeToAngle(90, MIDDLE + maxSpeed1, currentTime);
        if (currentTime - stepStartTime > 2000) {
            break;
        }
    }
    stepStartTime = millis();
}


void loop() {
    unsigned long currentTime{ millis() };
    if ((currentTime - stepStartTime > 0.2*stepTime) &&
        (currentTime - stepStartTime < 0.4*stepTime)) {

        motorL.writeToAngle(90 + 45, MIDDLE + maxSpeed1, currentTime);
        motorR.writeToAngle(270 - 45, MIDDLE - maxSpeed1, currentTime);
    } else {
        // motorL.writeToAngle(270, MIDDLE - maxSpeed1, currentTime);
        // motorR.writeToAngle(90, MIDDLE + maxSpeed1, currentTime);
        motorL.stop();
        motorR.stop();
    }

    if (currentTime - stepStartTime > stepTime) {
        stepStartTime = currentTime;
    }
}
