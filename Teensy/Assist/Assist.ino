/*
Main code to run the worm ankle exoskeleton.
Connects to both left and right exoskeletons.
*/
#include <Arduino.h>
#include <Metro.h>  // easy timers

#include "constants.h"

class Axo {
public:
    Axo(bool left) : m_timerFSR{timer::FSR},
                     m_timerLoad{timer::LOADCELL},
                     m_timerMotor{timer::MOTOR}
    {
        if (left) {
            m_PIN_FSR = pin::FSR_L;
            m_PIN_LOADCELL = pin::LOADCELL_L;
        } else {
            m_PIN_FSR = pin::FSR_R;
            m_PIN_LOADCELL = pin::LOADCELL_R;
        }
    }

    void begin() // starts up sensors. Call once in setup
    {
        /** MOTOR **/
        // Serial8.begin(9600); (TODO: FIGURE OUT HOW TO SEPARATE LEFT AND RIGHT MOTORS)
        // TODO: write upper and lower limits

        /** FSR **/
        pinMode(m_PIN_FSR, INPUT);
        /** LOAD CELL **/
        pinMode(m_PIN_LOADCELL, INPUT);

        // extra delay just to be safe
        delay(1000);
    }

    void start() // resets timers
    {
        m_startTime = millis();
    }

    void spinSensors() // Reads all metro timers
    {
        if (m_timerFSR.check()) {
            m_fsrVal = analogRead(m_PIN_FSR);

            // get the current time for step counting
            unsigned long t{ millis() };
            unsigned long dt{ t - m_startTime };
            // check for a step
            if (m_fsrVal > control::FSR_HIGH_THRESH) {
                if (!m_fsrHigh) {
                    m_fsrHigh = true;
                    m_stepTime = dt;
                    m_stepTimeAccum = control::ALPHA_STEP*m_stepTime + (1.0 - control::ALPHA_STEP)*m_stepTimeAccum;
                    m_startTime = t;
                }
            }
        }
        if (m_timerLoad.check()) {
            m_loadCellVal = analogRead(m_PIN_LOADCELL);
        }
    }

    double getGaitPercent() {
        unsigned long dt{ millis() - m_startTime };
        double percent = static_cast<double>(dt) / m_stepTimeAccum;
        return percent;
    }

    void setMotorAngle(int deg) // set desired output angle (degrees)
    {
        // converts output angle to motor angle
        constexpr int GEAR_RATIO{ 106 };
        deg *= GEAR_RATIO;
        Serial8.printf('[M] %d', deg);
    }

    void disableMotor() // stop the motor completely
    {
        Serial8.print('[D]');
    }

private:
    // Metro timers (for sensors)
    Metro m_timerFSR;
    Metro m_timerLoad;
    Metro m_timerMotor;

    // Pins
    short m_PIN_FSR;
    short m_PIN_LOADCELL;

    // FSR
    unsigned int m_fsrVal{};
    float m_fsrAccum{};
    bool m_fsrHigh{false};
    unsigned long m_stepTime{2000};
    unsigned long m_stepTimeAccum{2000};
    unsigned long m_startTime{};

    // Load cell
    unsigned int m_loadCellVal{};
};

// Control loop rate
Metro controlTimer{5}; // 200 Hz (5 ms)

// Create Axos
Axo axoL(true);
Axo axoR(false);


void setup() {
    // Setup serial
    Serial.begin(115200);
    Serial.setTimeout(500); // ms
    Serial.println("Compiled: " __DATE__ ", " __TIME__ );

    // start up sensors
    axoL.begin();
    axoR.begin();

    // Wait for a startup command
    Serial.println("Waiting for startup");
    while (1) {
        if (Serial.available()) {
            if (Serial.read() == 's') {
                Serial.println("Startup received!");
                break;
            }
        }
        // TODO: Add bluetooth start
        delay(10);
    }

    // reset control timer
    controlTimer.reset();
    axoL.start();
    axoR.start();
}

void loop() {
    if (controlTimer.check()) {
        // EXECUTE CONTROL
        bangBangAtPushoff(axoL, axoL.getGaitPercent());
        bangBangAtPushoff(axoR, axoR.getGaitPercent());
    }

    // Spin
    axoL.spinSensors();
    axoR.spinSensors();
}


static constexpr int maxAngle{200}; // degrees
static constexpr int zeroAngle{0};

// TODO: replace
float backPulsePercent{0.1};
float triggerPercent{0.3};
float motorOnPercent{0.35};

void bangBangAtPushoff(Axo& axo, double gaitPercent) {
    float a{ triggerPercent };                 // pulse starts
    float b{ a + motorOnPercent };             // pulse ends
    float c{ min(1.0, b + backPulsePercent)};         // buffer time for backwards pulse
    // SerialOut.printf("dt: %.4f, a: %.4f, b: %.4f, c: %.4f", deltat, a, b, c);

    // Follow a piecewise curve
    // BEFORE *TRIGGERPERCENT*
    if (gaitPercent < a) {
        // initially no motion
        axo.setMotorAngle(zeroAngle);
    } else if ((gaitPercent >= a) && (gaitPercent <= b)) {
        // spike at pushoff
        axo.setMotorAngle(maxAngle);
    } else if ((gaitPercent > b) && (gaitPercent <= c)) {
        // brief backwards pulse
        axo.setMotorAngle(-50);
    } else if (gaitPercent > c) {
        // return to no motion
        axo.setMotorAngle(zeroAngle);
    }
}