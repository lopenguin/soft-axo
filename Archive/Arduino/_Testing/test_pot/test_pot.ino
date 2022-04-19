const int potPin{21};

void setup() {
  pinMode(potPin, INPUT);
}

void loop() {
  int potVal = analogRead(potPin);
  Serial.println(potVal);
  delay(10);

}
