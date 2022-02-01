/*
Code to test the ESC powering a motor. Uses servo class.

Lorenzo Shaikewitz, 1/17/2022
*/
#include <PWMServo_Lorenzo.h>

const int MIDDLE{1522};
const int stepTime{1777};
const int maxSpeed1{65};
const int minSpeed1{40};
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
    void stop(int lastWriteVal) {
        if (m_lastWriteVal > MIDDLE) {
            writeMicroseconds(MIDDLE_LOW);
        } else {
            writeMicroseconds(MIDDLE_HIGH);
        }
        m_lastWriteVal = lastWriteVal;
    }

    // overloaded version that does not overwrite lastWriteVal
    void stop() {
        if (m_lastWriteVal > MIDDLE) {
            writeMicroseconds(MIDDLE_LOW);
        } else {
            writeMicroseconds(MIDDLE_HIGH);
        }
    }

    // writes the motor to the given speed (with safety stop)
    void write(int speed, unsigned long t) {
        // safety stop
        if (speed == m_lastWriteVal) {
            if ((t - m_lastWriteTime) > MAX_WRITE_TIME) {
                stop();
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
    const uint MAX_WRITE_TIME{250};
};


DroneMotor motorR(3, 22);  // create servo object to control a servo
DroneMotor motorL(4, 23);  // create servo object to control a servo

void setup() {
//  myservo.attach(SERVO_PIN_A);  // attaches the servo on pin 9 to the servo object
    motorR.attach(); // some motors need min/max setting
    motorL.attach(); // some motors need min/max setting
    delay(1000);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);
    delay(2000);

    stepStartTime = millis();
}


void loop() {
    unsigned long currentTime = millis();

    // Trigger the motors for a brief period
    if ((currentTime - stepStartTime > 0.2*stepTime) &&
        (currentTime - stepStartTime < 0.4*stepTime)) {
        // write to the motors
        motorR.write(MIDDLE - maxSpeed1, currentTime);
        motorL.write(MIDDLE + maxSpeed1, currentTime);

    // reset the step start time if step time is passed
    } else if (currentTime - stepStartTime > stepTime) {
        // reset step time
        stepStartTime = currentTime;

    } else if (currentTime - stepStartTime > 0.4*stepTime) {

        // trigger a brief backwards pulse of the motors
        if (currentTime - stepStartTime < min(0.5*stepTime, 0.4*stepTime + 150)) {
            // Short backwards movement
            motorR.write(MIDDLE + minSpeed1, currentTime);
            motorL.write(MIDDLE - minSpeed1, currentTime);
        } else {
            motorR.stop();
            motorL.stop();
        }

    // otherwise, stop the motors
    } else {
        motorR.stop(MIDDLE);
        motorL.stop(MIDDLE);
    }
}
