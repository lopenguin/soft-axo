/*
Holds an interface for the drone motors. Implements time-based safety checks.

Lorenzo Shaikewitz, 2/9/2022
*/
#ifndef DRONE_MOTOR_AXO_H
#define DRONE_MOTOR_AXO_H

const int MIDDLE{1522};

const int maxSpeed1{70};
const int minSpeed1{50};

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


private:
    const int m_motorPin;
    const int m_potPin;

    int m_lastWriteVal;
    unsigned long m_lastWriteTime{};


    const int MIDDLE_HIGH{1520};
    const int MIDDLE_LOW{1500};
    const uint MAX_WRITE_TIME{300};
};

#endif
