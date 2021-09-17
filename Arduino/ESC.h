/*
    ESC.h - Library for controlling BLDC ESCs with general microcontrollers
    Created by the Amber Lab at Caltech for an ankle-exoskeleton
*/

#ifndef ESC_h
#define ESC_h

#include "Arduino.h"

class ESC
{
private:
  int _pin;              // ESC digital pwm pin
  double _timer;         // used to emited well-timed squarewave
  int _offtime;          // minimum length of off time in duty cycle
  int _calibration_time; // time needed for calibration
  int _calibration_pwm;  // pwm needed for calibration

public:
  ESC(const int pin, const int offtime, const int calibration_time, const int calibration_pwm);

  /**
      * Uses timing to emit a pulsewidth wave the ESC can read without halting the looping code
      * @param dutytime the time in microseconds the pulse should be
      */
  void turn(int dutytime);

  /**
     * Emits a single pulsewidth wave
     * @param dutytime the time in microseconds the pulse should be
     */
  void pulsewidth(int dutytime);

  /**
     * Calibrates the ESC based on inputted parameters
     */
  void calib();
};

#endif