double x = 0;
double q = 0;
double lastq = 0;
String test;
bool dir = true;

// constants needed for state detection
// follows the following equation: Q(s) = -a(s - b) ^ 4 + d, a > 0
// this is meant to work with the very last quaternion digit (see excel graphs)
const float p = 4;
const float a = 4;
const float b = 0.5;
const float d = 0.15;

// future implementations include using other quaternion digits, and averaging state to
// get a more accurate prediction

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // triangle wave generation----
  q = 0.25 * x - 0.1;
  if (dir) {
    x += 0.005;
  } else {
    x -= 0.005;
  }
  if (x >= 1) {
    dir = false;
  } else if (x <= 0) {
    dir = true;
  }
  // -----------------------------
  Serial.print(state_based(q));
  Serial.print(" ");
  Serial.print(q);
  Serial.print(" ");
  Serial.println(lastq);
  lastq = q;  // this is the "derivative" section. In the future this will be replaced by
              // a rolling average
}

float state_based(float q) {
  int multiplier = 0;
  if (q - lastq > 0) { // utilizes the derivative to see which "side" of the curve we're on
    multiplier = -1;
  } else {
    multiplier = 1;
  }
  return multiplier * pow((d - q) / a, 1 / p) + b; // constants like a and d will adapt in future versions
}
