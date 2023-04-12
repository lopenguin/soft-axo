#include <SimpleFOC.h>
// software interrupt library

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(1);
BLDCDriver6PWM driver = BLDCDriver6PWM(A_PHASE_UH, A_PHASE_UL, A_PHASE_VH, A_PHASE_VL, A_PHASE_WH, A_PHASE_WL);
HardwareSerial mySerial(A_USART2_RX, A_USART2_TX);

HallSensor sensor(A_HALL1, A_HALL2, A_HALL3, 1);

//#123

// Interrupt routine intialisation
// channel A and B callbacks
void doA() {
  sensor.handleA();
}
void doB() {
  sensor.handleB();
}
void doC() {
  sensor.handleC();
}
// If no available hadware interrupt pins use the software interrupt

// angle set point variable
float target_angle = 0;
float upper_bound = 1000000;
float lower_bound = -1000000;
// instantiate the commander
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  mySerial.begin(9600);
  mySerial.setTimeout(10);
  // initialize sensor hardware
  sensor.init();
  sensor.enableInterrupts(doA, doB, doC); //, doC);
  // software interrupts
  // link the motor to the sensor
  motor.linkSensor(&sensor);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 18;
  driver.init();
  // link the motor and the driver
  motor.linkDriver(&driver);


  // aligning voltage [V]
  motor.voltage_sensor_align = 3;
  // index search velocity [rad/s]
  motor.velocity_index_search = 3;

  // set motion control loop to be used
  motor.controller = MotionControlType::angle;

  // contoller configuration
  // default parameters in defaults.h

  // velocity PI controller parameters
  motor.PID_velocity.P = 0.005;
  motor.PID_velocity.I = 0;
  motor.PID_velocity.D = 0;
  // default voltage_power_supply
  motor.voltage_limit = 16;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 300;

  // velocity low pass filtering time constant
  motor.LPF_velocity.Tf = 0.01;

  // angle P controller
  motor.P_angle.P = 18;
  //  maximal velocity of the position control
  motor.velocity_limit = 35000;


  // initialize motor
  motor.init();
  // align sensor and start FOC
  motor.initFOC();
  digitalWrite(LED_BUILTIN, HIGH);
}

void instruction(String cmd) {
  if (cmd.length() > 0) {
    char type = cmd.charAt(0);
    long value = cmd.substring(1).toInt();
    if (type == 'M') {
      if (value < upper_bound && value > lower_bound) {
        target_angle = value;
      }
    } else if (type == 'U') {
      upper_bound = value;
    } else if (type == 'L') {
      lower_bound = value;
    }
  }
}

void loop() {
  // main FOC algorithm function
  // the faster you run this function the better
  motor.loopFOC();

  //  motor.move(map(int(analogRead(A_POTENTIOMETER)),0,1023,-5000,5000));

  if (mySerial.available()) {
    String cmd = mySerial.readStringUntil(']');
    if (cmd.length() != 0 && cmd.charAt(0) == '[') {
      instruction(cmd.substring(1));
    }
  }

  motor.move(target_angle);
}
