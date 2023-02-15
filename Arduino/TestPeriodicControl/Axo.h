/*
Sensor and motor carrier class for ankle exoskeleton. Designed to abstract
essential but lower-level functions away from Powered.ino.

Lorenzo Shaikewitz, 4/19/2022
*/
#ifndef AXO_CLASS_H
#define AXO_CLASS_H

#include <Arduino.h>
#include <Metro.h>  // easy timers
#include <Servo.h>

#include "constants.h"

void updateMotors(void);

class Axo {
public:
    Axo() : m_timerIMU{timer::IMU},
            m_timerFSR{timer::FSR},
            m_timerLoad{timer::LOADCELL},
            m_timerMotor{timer::MOTOR},
            m_timerHeartbeat{timer::HEARTBEAT}
    {
        m_estopPressed = false; // E-STOP ISR variable
    }

    void begin(bool useIMUOffsets = true);  // starts up sensors. Call once in setup()
    void beginMotors();                     // starts up motors. Call once in setup()

    void printKey();        // prints sensor output key
    void heartbeat();       // prints a "heartbeat" (current time every 2 seconds)

    void setGreenLED(int val) {digitalWrite(pin::LEDG, val);}
    void setBlueLED(int val) {digitalWrite(pin::LEDB, val);}

    void spin();            // Reads all metro timers. Call in loop().

    void setAngle(int val); // sets motor desired angle. 100 is neutral. Range: (0, 800)
    void stopMotors();      // turns motors off without disconnecting them
    void detachMotors();    // disconnects motors/motor pins

    int getFSR()   { return m_fsrVal; }
    int getLoad()  { return m_loadCellVal; }

    bool resetEstop();  // reads estop and resets value to false

private:
    // Metro timers (for sensors)
    Metro m_timerIMU;
    Metro m_timerFSR;
    Metro m_timerLoad;
    Metro m_timerMotor;
    Metro m_timerHeartbeat;

    // Servo objects (for motors).
    Servo m_leftMotor;
    Servo m_rightMotor;

    // Motor update helper
    void updateMotors();
    bool m_useMotorMetro{};
    int m_targetAngleL{};
    int m_targetAngleR{};
    
    int m_prevPotL{};
    int m_prevPotR{};
    int m_turnsL{};
    int m_turnsR{};

    // IMU objects
    Adafruit_BNO055 m_shinIMU = Adafruit_BNO055(BNO055_ID, 0x29);
    Adafruit_BNO055 m_footIMU = Adafruit_BNO055(BNO055_ID, 0x28);

    // SENSOR OBJECTS
    // IMU: quats and calibration
    imu::Quaternion m_quatShin{};
    uint8_t m_quatCalShin[4]{};

    imu::Quaternion m_quatFoot{};
    uint8_t m_quatCalFoot[4]{};

    // FSR
    unsigned int m_fsrVal{};

    // Load cell
    unsigned int m_loadCellVal{};


    // SENSOR PRINT FUNCTIONS
    void printIMUs(sensors_event_t* shinAccel, sensors_event_t* footAccel);
    unsigned long m_startTime{};

    // interrupt callback for handling estop
    static volatile bool m_estopPressed;
    static void estopCallback();

public:
    static uint16_t floatToInt(float f, int precision = 4);
    unsigned long startTime() { return m_startTime; }
};

// Automatically writes the specified bytes to the Serial monitor
void writeBytes(int v, int numBytes = 2);

#endif
