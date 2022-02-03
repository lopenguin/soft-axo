/*
Final code for logging data from two IMUs to Teensy Prop Shield. Just data
logging of relative angle between the two IMUs (~11 Hz). There is no active
control.

Procedure:
- Change "runTimeSeconds" to change how long it runs for.
- Change "FILE_NAME" to change what name it is saved under. Note the program
appends "00" -> "99" to this file name. You may need to erase the chip
(EraseFlash) to free up space.
- Upload to teensy 3.2. Check wiring.
- Jump or open serial monitor to activate.
- Teensy LED will blink when logging complete.

To download: upload "teensytransfertool" to Teensy in RAW HID mode. Run
teensytransfer.exe in cmd. Copy over relevant files.

Lorenzo Shaikewitz, 2/1/2022
*/

#include <NXPMotionSense_Lorenzo.h>
#include "utility/NXPSensorRegisters.h"
#include <Wire.h>
#include <EEPROM.h>

#include <SerialFlash.h>
#include <SPI.h>

#include <PWMServo_Lorenzo.h>

#include "Axo.h"
#include "constants.h"

String FILE_NAME{"t"};
const int runTimeSeconds{300};
unsigned long startTime{};

/* MOTOR CLASS */
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

DroneMotor motorR(pin::MOTOR_R, pin::POT_R);  // create servo object to control a servo
DroneMotor motorL(pin::MOTOR_L, pin::POT_L);  // create servo object to control a servo
/* END MOTOR CLASS */

Axo axo(runTimeSeconds);

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // MOTOR SETUP
    motorR.attach(); // some motors need min/max setting
    motorL.attach(); // some motors need min/max setting
    delay(1000);
    motorR.writeMicroseconds(1500);
    motorL.writeMicroseconds(1500);
    delay(2000);

    axo.begin();
    // startup condition: wait for high boot tap.
    while (true && !Serial) {
        if (axo.propIMUAvail()) {
            axo.updatePropIMU();
            if (axo.started())
                // exit!
                break;
        }
        delay(10);
    }

    Message m = axo.beginFlash(FILE_NAME);
    switch (m) {
        case Message::OK:
            Serial.println("Axo started successfully!");
            break;
        case Message::NO_FLASH_CHIP:
            Serial.println("Flash chip not detected!");
            while (1) { blink(LED_BUILTIN); }
            break;
        case Message::NO_FLASH_SPACE:
            Serial.println("No flash space left! Please erase chip.");
            while (1) { blink(LED_BUILTIN); }
            break;
        case Message::FILE_TOO_LONG:
            Serial.println("File name is too long. Check constants.h for max length.");
            while (1) { blink(LED_BUILTIN); }
            break;
    }

    // PRINTING
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Saving data to: ");
    Serial.println(axo.getSavefile());
    Serial.println("-----");
    startTime = micros();
    stepStartTime = millis();
}

unsigned long lastTime{};
unsigned long lastIMUSaveTime{};
int stepCount{0};

void loop() {
    if (axo.propUpdated()) {
        if (axo.adaIMUAvail()) {
            // pretty consistently get ~10250 us per sample!
            // Serial.println(micros() - lastTime);
            lastTime = micros();
            axo.updateAdaIMU();
        }
    } else {
        if (axo.propIMUAvail()) {
            axo.updatePropIMU();
            // axo.printData();
            unsigned long currentTime{micros()};
            if (currentTime - lastTime > 10000) {
                // it's like there's an optimal delay here - not sure why
                // or how to get it, but this combo works.

                delayMicroseconds(300);
            }
            // wait 5 seconds to start saving data
            if (currentTime - startTime > 5000000) {
                if (!axo.saveData(currentTime - lastIMUSaveTime)) {
                    Serial.println("File space exceeded.");
                    pinMode(LED_BUILTIN, OUTPUT);
                    while (1) {
                        blink(LED_BUILTIN);
                    }
                }
                lastIMUSaveTime = micros();
                // Serial.println("saving...");
                // axo.printData();
                // axo.printRelQuat();

                // check if we've taken a step (this detects toe lift)
                if (axo.updateAverage()) {
                    unsigned long currentMSTime{millis()};
                    if (currentMSTime - stepStartTime > 800) {
                        Serial.printf("STEP %d: %d milliseconds\n", stepCount, (currentMSTime - stepStartTime));
                        stepCount++;
                        stepStartTime = currentMSTime;
                    }
                }
            }
        }
    }



    // MOTOR WRITING
    unsigned long currentTime = millis();

    // Trigger the motors for a brief period
    if ((currentTime - stepStartTime > 0.0*stepTime) &&
        (currentTime - stepStartTime < 0.2*stepTime)) {
        // write to the motors
        motorR.write(MIDDLE - maxSpeed1, currentTime);
        motorL.write(MIDDLE + maxSpeed1, currentTime);

    // reset the step start time if step time is passed
    } else if (currentTime - stepStartTime > stepTime) {
        // reset step time
        stepStartTime = currentTime;

    } else if (currentTime - stepStartTime > 0.2*stepTime) {

        // trigger a brief backwards pulse of the motors
        if (currentTime - stepStartTime < min(0.3*stepTime, 0.2*stepTime + 150)) {
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

void blink(int pin) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
}
