/*
 Servo.cpp - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
 Copyright (c) 2009 Michael Margolis.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* 
 
 A servo is activated by creating an instance of the Servo class passing the desired pin to the attach() method.
 The servos are pulsed in the background using the value most recently written using the write() method
 
 Note that analogWrite of PWM on pins associated with the timer are disabled when the first servo is attached.
 Timers are seized as needed in groups of 12 servos - 24 servos use two timers, 48 servos will use four.
 
 The methods are:
 
 Servo - Class for manipulating servo motors connected to Arduino pins.
 
 attach(pin )  - Attaches a servo motor to an i/o pin.
 attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
 default min is 544, max is 2400  
 
 write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
 writeMicroseconds() - Sets the servo pulse width in microseconds 
 read()      - Gets the last written servo pulse width as an angle between 0 and 180. 
 readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
 attached()  - Returns true if there is a servo attached. 
 detach()    - Stops an attached servos from pulsing its i/o pin. 
 
*/

#if defined(__arm__) && (defined(__IMXRT1052__) || defined(__IMXRT1062__))
// ******************************************************************************
// Teensy generic implementation, using IntervalTimer
// ******************************************************************************

#include <Arduino.h>
#include <IntervalTimer.h>
#include "AxoServo.h"
#include "debug/printf.h"

#define usToTicks(us)    ((us) * 16)
#define ticksToUs(ticks) ((ticks) / 16)
#define ticksToUs_f(t)   ((float)(t) * 0.0625f)

static uint32_t servo_active_mask = 0;
static uint32_t servo_allocated_mask = 0;
static uint8_t servo_pin[MAX_SERVOS];
static uint16_t servo_ticks[MAX_SERVOS];

static IntervalTimer timer;
static void isr(void);

AxoServo::AxoServo()
{
	uint16_t mask;

	servoIndex = 0;
	for (mask=1; mask < (1<<MAX_SERVOS); mask <<= 1) {
		if (!(servo_allocated_mask & mask)) {
			servo_allocated_mask |= mask;
			servo_active_mask &= ~mask;
			return;
		}
		servoIndex++;
	}
	servoIndex = INVALID_SERVO;
}

uint8_t AxoServo::attach(int pin)
{
	return attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t AxoServo::attach(int pin, int minimum, int maximum)
{
	if (servoIndex < MAX_SERVOS) {
		pinMode(pin, OUTPUT);
		servo_pin[servoIndex] = pin;
		servo_ticks[servoIndex] = usToTicks(DEFAULT_PULSE_WIDTH);
		servo_active_mask |= (1<<servoIndex);
		min_ticks = usToTicks(minimum);
		max_ticks = usToTicks(maximum);
		if ((IRQ_NUMBER_t)timer >= NVIC_NUM_INTERRUPTS) {
			timer.begin(isr, 10);
		}
	}
	return servoIndex;
}

void AxoServo::detach()
{
	if (servoIndex >= MAX_SERVOS) return;
	servo_active_mask &= ~(1<<servoIndex);
	servo_allocated_mask &= ~(1<<servoIndex);
	if (servo_active_mask == 0) {
		timer.end();
	}
}

void AxoServo::write(int value)
{
	if (servoIndex >= MAX_SERVOS) return;
	if (value >= MIN_PULSE_WIDTH) {
		writeMicroseconds(value);
		return;
	} else if (value > 180) {
		value = 180;
	} else if (value < 0) {
		value = 0;
	}
	if (servoIndex >= MAX_SERVOS) return;
	servo_ticks[servoIndex] = map(value, 0, 180, min_ticks, max_ticks);
}

void AxoServo::writeMicroseconds(int value)
{
	value = usToTicks(value);
	if (value < min_ticks) {
		value = min_ticks;
	} else if (value > max_ticks) {
		value = max_ticks;
	}
	if (servoIndex >= MAX_SERVOS) return;
	servo_ticks[servoIndex] = value;
}

int AxoServo::read() // return the value as degrees
{
	if (servoIndex >= MAX_SERVOS) return 0;
	return map(servo_ticks[servoIndex], min_ticks, max_ticks, 0, 180);
}

int AxoServo::readMicroseconds()
{
	if (servoIndex >= MAX_SERVOS) return 0;
	return ticksToUs(servo_ticks[servoIndex]);
}

bool AxoServo::attached()
{
	if (servoIndex >= MAX_SERVOS) return 0;
	return servo_active_mask & (1<<servoIndex);
}


void AxoServo::writeAngle(int value) 
{
	if (servoIndex >= MAX_SERVOS) return;

}


static void isr(void)
{
	static uint8_t channel=MAX_SERVOS;
	static uint8_t next_low=255;
	static uint32_t tick_accum=0;

	// If a pin is still HIGH from a prior run, turn it off
	if (next_low < 255) {
		digitalWrite(next_low, LOW);
	}

	// If we're on an active channel, drive it HIGH
	if (channel < MAX_SERVOS && (servo_active_mask & (1<<channel))) {
		uint8_t pin = servo_pin[channel];
		digitalWrite(pin, HIGH);
		next_low = pin;
	} else {
		next_low = 255;
	}

	// Generate an oscilloscope trigger pulse at beginning
	//if (channel == __builtin_ctz(servo_active_mask)) {
		//digitalWrite(2, HIGH);
		//delayMicroseconds(1);
		//digitalWrite(2, LOW);
	//}

	// Find the next channel and set the timer up
	if (++channel >= MAX_SERVOS) {
		channel = 0;
	}
	do {
		if (servo_active_mask & (1<<channel)) {
			uint32_t ticks = servo_ticks[channel];
			tick_accum += ticks;
			timer.update(ticksToUs_f(ticks));
			return;
		}
		channel++;
	} while (channel < MAX_SERVOS);

	// when all channels have output, wait for the refresh interval
	if (tick_accum < usToTicks(REFRESH_INTERVAL)) {
		timer.update(ticksToUs_f(usToTicks(REFRESH_INTERVAL) - tick_accum));
	} else {
		timer.update(ticksToUs_f(100));
	}
	tick_accum = 0;
}



#endif
