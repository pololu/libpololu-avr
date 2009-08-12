/*
  OrangutanServo.cpp - Library for generating servo control pulses with digital
	outputs on the Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  Note
	that only the Orangutan SV and SVP can supply enough current to power servos
	off of their regulated voltage.  All other devices can supply the control
	signals only (you must power the servos from a separate source).  This
	library relies on Timer1, so it will conflict with any other libraries that
	use Timer1 (e.g. OrangutanBuzzer).  You cannot use the OrangutanBuzzer
	library to play music while using the OrangutanServo library to control
	servos.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
 * Copyright (c) 2009 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OrangutanServos.h"


// global arrays for storing state data for each of the 8 possible servos

// the current position of the servo (i.e. the current pulse width in units of 0.1 us)
unsigned int servoPos[8]			= {0, 0, 0, 0, 0, 0, 0, 0};

// the desired final position of the servo (i.e. target pulse width in units of 0.1 us)
unsigned int servoTargetPos[8] 		= {0, 0, 0, 0, 0, 0, 0, 0};

// the amount by which the position is allowed to change every 20 ms, in units of 0.1 us
// a value of 0 means that speed control is disabled (servoPos[i] = servoTargetPos[i])
unsigned int servoSpeed[8]			= {0, 0, 0, 0, 0, 0, 0, 0};

// the index of the servo whose pulse is currently being generated
unsigned char idx;


#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)
struct PortStruct portPin[3];		// one pin for each mux selector pin (servo signal generated on separate pin PD5)
#else
struct PortStruct portPin[8];		// one pin for each servo (servo signal will be generated on these pins directly)
#endif


// This interrupt is executed when Timer1 counter (TCNT1) = TOP (ICR1) and the value in OCR1A (the next duty cycle)
// has been loaded.
ISR(TIMER1_CAPT_vect)
{
	unsigned char i;
	idx = (idx + 1) & 7;					// increment idx, loop back to 0 when idx == 8

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)

	unsigned char temp = idx;	// set mux pins based on bits of idx (pin SA = idx bit 0, ..., SC = idx bit 2)
	for (i = 0; i < 3; i++)
	{
		if (temp & 1)
		{
			*portPin[i].portRegister |= portPin[i].bitmask;
		}
		else
		{
			*portPin[i].portRegister &= ~portPin[i].bitmask;
		}
		temp >>= 1;
	}

#endif
	
	i = (idx + 1) & 7;
	unsigned int pos = servoPos[i];		// hint to the compiler that it should store this RAM value in registers
	if (servoSpeed[i])
	{
		if (servoTargetPos[i] > pos)
		{
			pos += servoSpeed[i];
			if (pos > servoTargetPos[i])
				pos = servoTargetPos[i];
		}
		else
		{
			if (pos < servoTargetPos[i] + servoSpeed[i])
				pos = servoTargetPos[i];
			else
				pos -= servoSpeed[i];
		}
	}
	else
	{
		pos = servoTargetPos[i];
	}
	OCR1A = pos;							// setup duty cycle for next servo now; will take effect just before this ISR is next called
	servoPos[i] = pos;	
}


#if !defined (__AVR_ATmega324P__) || !defined (__AVR_ATmega1284P__)	// NOT USED FOR ORANGUTAN SVP, which uses hardware PWM
// This interrupt is executed when Timer1 counter (TCNT1) = OCR1A.  Since we are running Timer1 in phase-correct mode,
// TCNT1 counts from 0 up to TOP and then back down to 0 again.  As a result, this interrupt will occur twice (once
// while the timer is upcounting and once while it is downcounting) for every TIMER1_CAPT interrupt.
// We use this interrupt to generate the servo output signals in software on the digital outputs associated with each
// servo.
ISR(TIMER1_COMPA_vect)
{
	*(portPin[idx].portRegister) ^= portPin[idx].bitmask;
}
#endif


#ifdef LIB_POLOLU

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)
extern "C" void init_servos(unsigned char SA, unsigned char SB, unsigned char SC)
{
	OrangutanServos::initServos(SA, SB, SC);
}
#else
extern "C" void init_servos(const unsigned char servoPins[], unsigned char numServos)
{
	OrangutanServos::initServos(servoPins, numServos);
}
#endif

extern "C" unsigned int get_servo_position(unsigned char servoNum)
{
	return OrangutanServos::getServoPosition(servoNum);
}

extern "C" void set_servo_target(unsigned char servoNum, unsigned int pos_us)
{
	OrangutanServos::setServoTarget(servoNum, pos_us);
}

extern "C" unsigned int get_servo_target(unsigned char servoNum)
{
	return OrangutanServos::getServoTarget(servoNum);
}

extern "C" void set_servo_speed(unsigned char servoNum, unsigned int speed)
{
	OrangutanServos::setServoSpeed(servoNum, speed);
}

extern "C" unsigned int get_servo_speed(unsigned char servoNum)
{
	return OrangutanServos::getServoSpeed(servoNum);
}

#endif



// constructor
OrangutanServos::OrangutanServos()
{

}


// initializes the global servo pin array with the specified pins, and configures the
// timer1 hardware module for generating the appropriate servo pulse signals.
// The Orangutan SVP version of this function takes three arguments (mux selection pins; the
// servo signal is output on pin PD5, which is the output of the mux)
// and uses only one interrupt (when TCNT1 = TOP (ICR1)) while the Orangutan SV, LV, Baby Orangutan, and 3pi version
// of this function take an array representing up to 8 pins (the pins on which to output
// the servo signals) and uses two interrupts (when TCNT1 = TOP (ICR1) and when TCNT1 = OCR1A).
#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)
void OrangutanServos::initServos(unsigned char SA, unsigned char SB, unsigned char SC)
{
	initPortPin(&portPin[0], SA);			// here we use the port pins as mux selectors
	initPortPin(&portPin[1], SB);
	initPortPin(&portPin[2], SC);

	PORTD &= ~(1 << PORTD5);	// MUX output pin -> low output or high-impedance input
	DDRD |= 1 << PORTD5;		// pin -> output

	TCCR1A = 0b10000010;		// clear OC1A on comp match when upcounting, set OC1A on comp match when downcounting

#else
void OrangutanServos::initServos(const unsigned char servoPins[], unsigned char numServos)
{
	unsigned char i;
	for (i = 0; i < numServos; i++)
	{
		initPortPin(&portPin[i], servoPins[i]);	// here we use the port pins as servo signal outputs
	}
	for (i = numServos; i < 8; i++)
	{
		initPortPin(&portPin[i], 255);
	}

#endif	
	idx = 0;

	TCCR1A = 0b00000010;		// disconnect OC1A and OC1B, configure for phase correct PWM (with TCCR1B)
	TCCR1B = 0b00010001;		// phase correct PWM with TOP = ICR1, clock prescaler = 1 (freq = FCPU / (2 * ICR1))

	ICR1 = 25000;				// 400 Hz PWM (2.5 ms period)
	TIMSK1 |= (1 << ICIE1);		// enable T1 input capture interrupt (occurs at TOP, when buffered duty cycle is loaded)
	sei();
}


// get the current width of the pulse (in us) being supplied to the specified servo.
// This method does not rely on feedback from the servo, so if the servo
// is being restrained or overly torqued, it might not return the actual
// position of the servo.  If you have speed limiting enabled, you can
// use this method to determine when the servo pulse signals have reached
// their desired target widths.
unsigned int OrangutanServos::getServoPosition(unsigned char servoNum)
{
	return (servoPos[servoNum] + 5) / 10;
}



// send a position value of 0 to turn off the specified servo.  Otherwise, valid
// target positions are between 400 and 2450 us.
void OrangutanServos::setServoTarget(unsigned char servoNum, unsigned int pos_us)
{
	if (pos_us > 2450)			// will get bad results if pulse is 100% duty cycle (2500)
		pos_us = 2450;
#if !defined (__AVR_ATmega324P__) && !defined (__AVR_ATmega1284P__)
	if (pos_us < 400)
		pos_us = 400;
#endif
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoTargetPos[servoNum & 7] = pos_us * 10;
	TIMSK1 |= 1 << ICIE1;
}


// get the target position (pulse width in us) of the specified servo.
unsigned int OrangutanServos::getServoTarget(unsigned char servoNum)
{
	return (servoTargetPos[servoNum & 7] + 5) / 10;
}


// speed parameter is in units of 100ns (1/10th of a microsecond)
// the servo position will be incremented or decremented by "speed"
// every 20 ms.
void OrangutanServos::setServoSpeed(unsigned char servoNum, unsigned int speed)
{
	if (speed > 25000)			// limit speed so it won't cause overflow problems when added to position
		speed = 25000;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoSpeed[servoNum & 7] = speed;
	TIMSK1 |= 1 << ICIE1;
}


// get the speed of the specified servo (the amount in tenths of a microsecond
// that the servo position is incremented or decremented every 20 ms).
unsigned int OrangutanServos::getServoSpeed(unsigned char servoNum)
{
	return servoSpeed[servoNum & 7];
}



// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
