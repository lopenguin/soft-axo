/*
Code for time-based control of ankle exoskeleton. Bang-Bang implemented.

Lorenzo Shaikewitz, 7/23/2021
*/
namespace pin {
    // motor h-bridge connections
    const short MOTOR_IN1{3};
    const short MOTOR_IN2{4};

    const short MOTOR_EN{5};

    const short JOY_BUTTON{21};
}

// gait variables
float startPercent{ 50 };
float endPercent{ 80 };
int maxPower{ 255 };
bool aborted{ 0 };

// timekeeping variables
unsigned int gaitTime_us{ 2000000 };
float gaitPercent{ 0 };

unsigned long startTime_us{0};
unsigned long currentTime_us{0};

void writeMotor(int speed, bool coast = false);

void setup() {
    Serial.begin(9600);

    pinMode(pin::MOTOR_IN1, OUTPUT);
    pinMode(pin::MOTOR_IN2, OUTPUT);
    pinMode(pin::MOTOR_EN, OUTPUT);
    digitalWrite(pin::MOTOR_EN, LOW);   // disables motor.

    pinMode(pin::JOY_BUTTON, INPUT_PULLUP);

    while (1) {
        if ((digitalRead(pin::JOY_BUTTON) == LOW) || Serial) {
            break;
        }
        delay(50);
    }
    Serial.println("Beginning motion.");

    attachInterrupt(digitalPinToInterrupt(pin::JOY_BUTTON), toggleAbort, RISING);

    startTime_us = micros();
    digitalWrite(pin::MOTOR_EN, HIGH);
}

void loop() {
    if (!aborted) {
        currentTime_us = micros();
        gaitPercent = (float)(currentTime_us - startTime_us) / gaitTime_us * 100;

        if (gaitPercent > 100) {
            startTime_us = currentTime_us;
            writeMotor(0);
        }

        if (gaitPercent > endPercent) {
            rampDown(gaitPercent - endPercent);
        } else if (gaitPercent > startPercent) {
            writeMotor(255);
        }
    }

    delay(5);
}


void writeMotor(int speed, bool coast) {
    if (coast) {
        digitalWrite(pin::MOTOR_EN, LOW);
    } else {
        digitalWrite(pin::MOTOR_EN, HIGH);
    }
    if (speed > 255 || speed < -255) {
        return;
    }

    analogWrite(pin::MOTOR_IN1, 0);
    analogWrite(pin::MOTOR_IN2, 0);

    if (speed > 0) {
        analogWrite(pin::MOTOR_IN1, speed);
    } else {
        analogWrite(pin::MOTOR_IN2, -speed);
    }
}

void toggleAbort() {
    if (!aborted) {
        aborted = 1;
    }
    else {
        aborted = 0;
        startTime_us = micros();
    }
}

void rampDown(float percentSince) {
    if (percentSince > 5) {
        writeMotor(0);
    } else {
        // try out an exponential decay to -255
        // int motorWrite = static_cast<int>(pow(2, -percentSince / 10) * 510 - 255);
        writeMotor(-255);
    }
}
