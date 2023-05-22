HardwareSerial* motor = &Serial5;

void setup() {
    Serial.begin(115200);

    // startup motor serial
    motor->begin(9600);
}

void loop() {
    if (motor->available() > 0) {
        motor->read();
    } if (Serial.available() > 0) {
        motor->write(Serial.read());
    }
}