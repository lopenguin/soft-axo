/*
 * Sketch to attempt safer control of BLDC motors on ankle exoskeleton. Make note
 * to switch directionality of motor if bugs arise.
 * 
 * Amber Lab 4/17/2022, Lorenzo and Neil
*/

#include <Servo.h>


// CONSTANTS-------------------------------------------------------------------
const int MOTOR_PIN = 4;
const int ENCODER_PIN = A8;

// ESC CONSTANTS
const int CALIBRATION_PWM = 1500;
const int CALIBRATION_TIME = 7000;
const int CONTROL_RATE = 100;       // in HZ (approximate, will likely be slower)

// POTENTIOMETER CONSTANTS
const int POT_ZERO = 0;             // lowest potentiometer value achievable
const int POT_MAX = 1023;           // highest potentiometer valye achievable
const int NOISE_JUMP = 500;         // minimum jump in potentiometer values to define a whole turn

// CONTROL CONSTANTS
const double C = 0.005;             // see Desmos graph for intuition for C
const double E = 2.71828;           // the natural constant e
const double K_p = 0.7;             // proportional control constant
const int MAX_FORWARD = 1600;       // maximum permitted forward PWM (m_f)
const int MAX_BACKWARD = 1400;      // maximum permitted backward PWM (m_b)
const int CHANGE_RATE = 1000;       // the interval at which new goal values are generated (milliseconds)


// DYNAMIC VARIABLES-----------------------------------------------------------
Servo motor;        // motor Servo object
int curr = 0;       // current potentiometer position
int prev = 0;       // potentiometer position from last loop
int turns = 0;      // number of whole turns completed by potentiometer
int goal = 0;       // goal position, alwasy should be in [POT_ZERO, POT_MAX]
float timer = 0;    // timer variable (in milliseconds)


void setup() {
  // Assign pins begin serial...
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  motor.attach(MOTOR_PIN);
  delay(1000);

  // Calibrate ESC...LED_BUILTIN will turn on after this step
  motor.writeMicroseconds(CALIBRATION_PWM);
  delay(CALIBRATION_TIME);

  // Set other variables up...
  digitalWrite(LED_BUILTIN, HIGH);
  prev = (int)analogRead(A8);
  timer = millis();
  goal = (int)(random(POT_ZERO, POT_MAX));
}

void loop() {
  // Generate value to target (random between POT_ZERO and POT_MAX)
  if (millis() - timer > CHANGE_RATE) {
    goal = (int)(random(POT_ZERO , POT_MAX));
    timer = millis();
  }

  // Control to goal using p control
  motor.writeMicroseconds(p(goal));

  // Read and interpret current pot position. Try to estimate if a whole turn has occurred
  curr = (int)analogRead(A8);
  if (curr - prev < -1 * NOISE_JUMP) {
    turns++;
  } else if (curr - prev > NOISE_JUMP) {
    turns--;
  }
  prev = curr;

  // Print and wait
  print_info();
  delay(1000.0 / CONTROL_RATE);
}

/*
 * Prints info from global variables
 *
 * @params: None
 * @return: None
*/
void print_info() {
  Serial.print(goal);
  Serial.print(" ");
  Serial.println(get_pos());
}


/*
 * P-control using safer control tactic (see Desmos plot in README)
 *
 * @params
 *  - desired:  the desired potentiometer value to reach
 * 
 * @return:
 *  - (int):    the PWM value to output to the ESC
*/
int p(int desired) {
  int ret = 1500 + (desired - get_pos()) * K_p;
  ret += pow(E,1 / (C * get_pos())); 
  ret -= pow(E, 1 / (C * (POT_MAX - get_pos())));
  return min(MAX_FORWARD, max(MAX_BACKWARD, ret));
}


/*
 * Gets the 'global' position of the output shaft (tries to estimate whole turns)
 *
 * @params:   None
 * 
 * @return
 *  - (int):  the estimated global shaft position
*/
int get_pos() {
  int ret = curr + 1023 * turns;
  return max(0, min(1023, ret));
}
