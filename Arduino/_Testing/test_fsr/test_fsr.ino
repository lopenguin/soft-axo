const int fsrPin{17};
const int stepThresh{250};  // threshold for heel contact

bool fsrTriggered{0};
int stepCount{0};
unsigned long stepStartTime{0};

void setup() {
  stepStartTime = millis();
}

void loop() {
  int fsrVal = analogRead(fsrPin);
  if (fsrVal > stepThresh) {
//    Serial.print(600);
    if (!fsrTriggered) {
      ++stepCount;
      unsigned long stepTime = millis() - stepStartTime;
      Serial.printf("Step %d: %d ms\n", stepCount, stepTime);
      stepStartTime = millis();
    }
    
    fsrTriggered = true;
  } else {
//    Serial.print(0);
    fsrTriggered = false;
  }
//  Serial.printf(", %d\n",fsrVal);
  delay(10);

}
