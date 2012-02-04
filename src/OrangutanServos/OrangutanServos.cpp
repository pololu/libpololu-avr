/*
  OrangutanServo.cpp - Library for generating servo control pulses with digital
	outputs on the Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  Note
	that only the Orangutan SV and SVP can supply enough current to power servos
	off of their regulated voltage.  All other devices can supply the control
	signals only (you must power the servos from a separate source).  This
	library relies on Timer1, so it will conflict with any other libraries that
	use Timer1 (e.g. OrangutanBuzzer).  You cannot use the OrangutanBuzzer
	library to play music while using the OrangutanServo library to control
	servos.
	
	This library can generate up to 16 servo control pulses.  On the Orangutan
	SVP, eight of these pulses must be via the servo pulse mux output.  The other
	eight are optional servo pulse outputs on digital I/O pins themselves.  On
	all other devices, all sixteen servo outputs are on digital I/O pins.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
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
#include <stdlib.h>
#include "OrangutanServos.h"
#include "../OrangutanResources/include/OrangutanModel.h"

// global arrays for storing state data for each of the 8 possible servos

// the current position of the servo (i.e. the current pulse width in units of 0.1 us)
unsigned int *servoPos;
unsigned int *servoPosB;

// the desired final position of the servo (i.e. target pulse width in units of 0.1 us)
unsigned int *servoTargetPos;
unsigned int *servoTargetPosB;

// the amount by which the position is allowed to change every 20 ms, in units of 0.1 us
// a value of 0 means that speed control is disabled (servoPos[i] = servoTargetPos[i])
unsigned int *servoSpeed;
unsigned int *servoSpeedB;

#ifdef _ORANGUTAN_SVP
unsigned char numMuxPins;	// number of mux control pins used (must be <= 3)
#endif

// the number of servos for which pulses are to be generated (must be <= 8)
unsigned char numServos;		// number of servos controlled by OCR1A duty cycles
unsigned char numServosB;		// number of servos controlled by OCR1B duty cycles

// the index of the servo whose pulse is currently being generated
unsigned char servoIdx;

struct PortStruct *portPin;		// mux selector pins (Orangutan SVP) or servo signal pins (everything else)
struct PortStruct *portPinB;	// servo signal pins


// This interrupt is executed when Timer1 counter (TCNT1) = TOP (ICR1) and the value in OCR1A (the next duty cycle)
// has been loaded.
ISR(TIMER1_CAPT_vect)
{
	unsigned char i;
	servoIdx = (servoIdx + 1) & 7;					// increment idx, loop back to 0 when idx == 8

#ifdef _ORANGUTAN_SVP

	unsigned char temp = servoIdx;	// set mux pins based on bits of idx (pin SA = idx bit 0, ..., SC = idx bit 2)
	for (i = 0; i < numMuxPins; i++)
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
	
	i = (servoIdx + 1) & 7;
	if (i >= numServos)
	{
			OCR1A = 0;
	}
	else
	{
		unsigned int pos = servoPos[i];		// hint to the compiler that it should store this RAM value in registers
		if (servoSpeed[i] && pos && servoTargetPos[i])
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
		OCR1A = pos;						// setup duty cycle for next servo now; will take effect just before this ISR is next called
		servoPos[i] = pos;
	}
	
	if (i >= numServosB)
	{
		OCR1B = 0;
	}
	else
	{
		unsigned int posB = servoPosB[i];		// hint to the compiler that it should store this RAM value in registers
		if (servoSpeedB[i])
		{
			if (servoTargetPosB[i] > posB)
			{
				posB += servoSpeedB[i];
				if (posB > servoTargetPosB[i])
					posB = servoTargetPosB[i];
			}
			else
			{
				if (posB < servoTargetPosB[i] + servoSpeedB[i])
					posB = servoTargetPosB[i];
				else
					posB -= servoSpeedB[i];
			}
		}
		else
		{
			posB = servoTargetPosB[i];
		}
		OCR1B = posB;						// setup duty cycle for next servo now; will take effect just before this ISR is next called
		servoPosB[i] = posB;
	}
#if !defined(_ORANGUTAN_SVP)
	if (servoIdx < numServos)
		*(portPin[servoIdx].portRegister) &= ~portPin[servoIdx].bitmask;
#endif
	if (servoIdx < numServosB)
		*(portPinB[servoIdx].portRegister) &= ~portPinB[servoIdx].bitmask;
}


#if !defined(_ORANGUTAN_SVP)	// NOT USED FOR ORANGUTAN SVP, which uses hardware PWM
// This interrupt is executed when Timer1 counter (TCNT1) = OCR1A.  Since we are running Timer1 in phase-correct mode,
// TCNT1 counts from 0 up to TOP and then back down to 0 again.  As a result, this interrupt will occur twice (once
// while the timer is upcounting and once while it is downcounting) for every TIMER1_CAPT interrupt.
// We use this interrupt to generate the servo output signals in software on the digital outputs associated with each
// servo.
ISR(TIMER1_COMPA_vect)
{
	if (servoIdx < numServos)
	{
		*(portPin[servoIdx].portRegister) ^= portPin[servoIdx].bitmask;
	}
}
#endif

// This interrupt is executed when Timer1 counter (TCNT1) = OCR1B.  Since we are running Timer1 in phase-correct mode,
// TCNT1 counts from 0 up to TOP and then back down to 0 again.  As a result, this interrupt will occur twice (once
// while the timer is upcounting and once while it is downcounting) for every TIMER1_CAPT interrupt.
// We use this interrupt to generate the servo output signals in software on the digital outputs associated with each
// servo.
ISR(TIMER1_COMPB_vect)
{
	if (servoIdx < numServosB)
	{
		*(portPinB[servoIdx].portRegister) ^= portPinB[servoIdx].bitmask;
	}
}


// use of init() is discouraged; use start() instead
extern "C" unsigned char servos_init(const unsigned char servoPins[], unsigned char numPins)
{
	return OrangutanServos::start(servoPins, numPins, 0, 0);
}

// use of init() is discouraged; use start() instead
extern "C" unsigned char servos_init_extended(const unsigned char servoPins[], unsigned char numPins, const unsigned char servoPinsB[], unsigned char numPinsB)
{
	return OrangutanServos::start(servoPins, numPins, servoPinsB, numPinsB);
}

extern "C" unsigned char servos_start(const unsigned char servoPins[], unsigned char numPins)
{
	return OrangutanServos::start(servoPins, numPins, 0, 0);
}

extern "C" unsigned char servos_start_extended(const unsigned char servoPins[], unsigned char numPins, const unsigned char servoPinsB[], unsigned char numPinsB)
{
	return OrangutanServos::start(servoPins, numPins, servoPinsB, numPinsB);
}

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


extern "C" unsigned int get_servo_positionB(unsigned char servoNum)
{
	return OrangutanServos::getServoPositionB(servoNum);
}

extern "C" void set_servo_targetB(unsigned char servoNum, unsigned int pos_us)
{
	OrangutanServos::setServoTargetB(servoNum, pos_us);
}

extern "C" unsigned int get_servo_targetB(unsigned char servoNum)
{
	return OrangutanServos::getServoTargetB(servoNum);
}

extern "C" void set_servo_speedB(unsigned char servoNum, unsigned int speed)
{
	OrangutanServos::setServoSpeedB(servoNum, speed);
}

extern "C" unsigned int get_servo_speedB(unsigned char servoNum)
{
	return OrangutanServos::getServoSpeedB(servoNum);
}

extern "C" void servos_stop()
{
	OrangutanServos::stop();
}


// constructor
OrangutanServos::OrangutanServos()
{

}


void freeServoMemory()
{
	if (portPin)
	{
		free(portPin);
		portPin = 0;
	}
	if (servoPos)
	{
		free(servoPos);
		servoPos = 0;
	}
	if (servoTargetPos)
	{
		free(servoTargetPos);
		servoTargetPos = 0;
	}	
	if (servoSpeed)
	{
		free(servoSpeed);
		servoSpeed = 0;
	}
	
	if (portPinB)
	{
		free(portPinB);
		portPinB = 0;
	}
	if (servoPosB)
	{
		free(servoPosB);
		servoPosB = 0;
	}
	if (servoTargetPosB)
	{
		free(servoTargetPosB);
		servoTargetPosB = 0;
	}	
	if (servoSpeedB)
	{
		free(servoSpeedB);
		servoSpeedB = 0;
	}
}


// the destructor frees up allocated memory
OrangutanServos::~OrangutanServos()
{
	freeServoMemory();
}


// initializes the global servo pin array with the specified pins, and configures the
// timer1 hardware module for generating the appropriate servo pulse signals.
// The Orangutan SVP version of this function takes an array of mux selection pins; the
// servo signal is output on pin PD5, which is the output of the mux)
// and uses only one interrupt (when TCNT1 = TOP (ICR1)) while the Orangutan SV, LV, Baby Orangutan, and 3pi version
// of this function take an array representing up to 8 pins (the pins on which to output
// the servo signals) and uses two interrupts (when TCNT1 = TOP (ICR1) and when TCNT1 = OCR1A).
// since the servo code can control up to 16 servos using timer 1, there is an optional second
// set of parameters that allows the user to specify up to 8 more servos.  The servoPinsB array
// represents a set of up to eight digital I/O pins on which the servo signals should be output.
// If you don't want this second set of eight servos, use a numPinsB value of 0 (and you can pass in NULL for servoPinsB).
// A nonzero return value indicates that memory for the desired arrays could not be allocated
extern unsigned char buzzerInitialized;
extern volatile unsigned char buzzerFinished;
extern const char *buzzerSequence;


unsigned char OrangutanServos::start(const unsigned char *servoPins, unsigned char numPins, const unsigned char *servoPinsB, unsigned char numPinsB)
{
	TIMSK1 = 0;					// disable all timer1 interrupts

	buzzerInitialized = 0;
	buzzerFinished = 1;
	buzzerSequence = 0;
	
	TCCR1B = 0;

#ifdef _ORANGUTAN_SVP
	if (numPins > 3)
		numPins = 3;
	numMuxPins = numPins;
	numServos = 1 << numPins;
	
	// Drive the DEMUX input pin low.
	PORTD &= ~(1 << PORTD5);
	DDRD |= 1 << PORTD5;

	TCCR1A = 0b10000010;		// clear OC1A on comp match when upcounting, set OC1A on comp match when downcounting
#else
	if (numPins > 8)
		numPins = 8;
	numServos = numPins;
	
	TCCR1A = 0b00000010;		// disconnect OC1A and OC1B, configure for phase correct PWM (with TCCR1B)
#endif

	if (numPinsB > 8)
		numPinsB = 8;
	numServosB = numPinsB;

	freeServoMemory();
	
	portPin = (struct PortStruct*)malloc(sizeof(struct PortStruct)*numPins);
	servoPos = (unsigned int*)malloc(sizeof(unsigned int)*numServos);
	servoTargetPos = (unsigned int*)malloc(sizeof(unsigned int)*numServos);
	servoSpeed = (unsigned int*)malloc(sizeof(unsigned int)*numServos);
	if (portPin == 0 || servoPos == 0 || servoTargetPos == 0 || servoSpeed == 0)
	{
		freeServoMemory();
		return 1;
	}
	
	if (numPinsB)
	{
		portPinB = (struct PortStruct*)malloc(sizeof(struct PortStruct)*numPinsB);
		servoPosB = (unsigned int*)malloc(sizeof(unsigned int)*numServosB);
		servoTargetPosB = (unsigned int*)malloc(sizeof(unsigned int)*numServosB);
		servoSpeedB = (unsigned int*)malloc(sizeof(unsigned int)*numServosB);
		if (portPinB == 0 || servoPosB == 0 || servoTargetPosB == 0 || servoSpeedB == 0)
		{
			freeServoMemory();
			return 1;
		}
	}

	unsigned char i;
	for (i = 0; i < numPins; i++)
	{
		initPortPin(&portPin[i], servoPins[i]);
	}
	for (i = 0; i < numServos; i++)
	{
		servoPos[i] = 0;
		servoTargetPos[i] = 0;
		servoSpeed[i] = 0;
	}
	
	for (i = 0; i < numPinsB; i++)
	{
		initPortPin(&portPinB[i], servoPinsB[i]);
		servoPosB[i] = 0;
		servoTargetPosB[i] = 0;
		servoSpeedB[i] = 0;
	}

	servoIdx = 0;

	TCCR1B = 0b00010001;		// phase correct PWM with TOP = ICR1, clock prescaler = 1 (freq = FCPU / (2 * ICR1))

	ICR1 = 25000;				// 400 Hz PWM (2.5 ms period)
	TIFR1 = 0xFF;				// clear any pending timer1 interrupts
	TIMSK1 |= 1 << ICIE1;		// enable T1 input capture interrupt (occurs at TOP, when buffered duty cycle is loaded)
	if (numPinsB)
	{
		TIMSK1 |= 1 << OCIE1B;	// enable compare match B interrupt
	}
#if !defined(_ORANGUTAN_SVP)
	TIMSK1 |= 1 << OCIE1A;		// enable compare match A interrupt if not using Orangutan SVP
#endif
	sei();
	
	return 0;
}


// get the current width of the pulse (in us) being supplied to the specified servo.
// This method does not rely on feedback from the servo, so if the servo
// is being restrained or overly torqued, it might not return the actual
// position of the servo.  If you have speed limiting enabled, you can
// use this method to determine when the servo pulse signals have reached
// their desired target widths.
unsigned int OrangutanServos::getServoPosition(unsigned char servoNum)
{
	if (servoNum >= numServos)
		return 0;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted while reading position	
	unsigned int pos = (servoPos[servoNum] + 5) / 10;
	TIMSK1 |= 1 << ICIE1;
	return pos;
}



// send a position value of 0 to turn off the specified servo.  Otherwise, valid
// target positions are between 400 and 2450 us.
void OrangutanServos::setServoTarget(unsigned char servoNum, unsigned int pos_us)
{
	if (servoNum >= numServos)
		return;
	if (pos_us > 2450)			// will get bad results if pulse is 100% duty cycle (2500)
		pos_us = 2450;

	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoTargetPos[servoNum & 7] = pos_us * 10;
	TIMSK1 |= 1 << ICIE1;
}


// get the target position (pulse width in us) of the specified servo.
unsigned int OrangutanServos::getServoTarget(unsigned char servoNum)
{
	if (servoNum >= numServos)
		return 0;
	return (servoTargetPos[servoNum & 7] + 5) / 10;
}


// speed parameter is in units of 100ns (1/10th of a microsecond)
// the servo position will be incremented or decremented by "speed"
// every 20 ms.
void OrangutanServos::setServoSpeed(unsigned char servoNum, unsigned int speed)
{
	if (servoNum >= numServos)
		return;
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
	if (servoNum >= numServos)
		return 0;
	return servoSpeed[servoNum & 7];
}



// get the current width of the pulse (in us) being supplied to the specified servo.
// This method does not rely on feedback from the servo, so if the servo
// is being restrained or overly torqued, it might not return the actual
// position of the servo.  If you have speed limiting enabled, you can
// use this method to determine when the servo pulse signals have reached
// their desired target widths.
unsigned int OrangutanServos::getServoPositionB(unsigned char servoNum)
{
	if (servoNum >= numServosB)
		return 0;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted while reading position	
	unsigned int pos = (servoPosB[servoNum] + 5) / 10;
	TIMSK1 |= 1 << ICIE1;
	return pos;
}



// send a position value of 0 to turn off the specified servo.  Otherwise, valid
// target positions are between 400 and 2450 us.
void OrangutanServos::setServoTargetB(unsigned char servoNum, unsigned int pos_us)
{
	if (servoNum >= numServosB)
		return;
	if (pos_us > 2450)			// will get bad results if pulse is 100% duty cycle (2500)
		pos_us = 2450;

	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoTargetPosB[servoNum & 7] = pos_us * 10;
	TIMSK1 |= 1 << ICIE1;
}


// get the target position (pulse width in us) of the specified servo.
unsigned int OrangutanServos::getServoTargetB(unsigned char servoNum)
{
	if (servoNum >= numServosB)
		return 0;
	return (servoTargetPosB[servoNum & 7] + 5) / 10;
}


// speed parameter is in units of 100ns (1/10th of a microsecond)
// the servo position will be incremented or decremented by "speed"
// every 20 ms.
void OrangutanServos::setServoSpeedB(unsigned char servoNum, unsigned int speed)
{
	if (servoNum >= numServosB)
		return;
	if (speed > 25000)			// limit speed so it won't cause overflow problems when added to position
		speed = 25000;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoSpeedB[servoNum & 7] = speed;
	TIMSK1 |= 1 << ICIE1;
}


// get the speed of the specified servo (the amount in tenths of a microsecond
// that the servo position is incremented or decremented every 20 ms).
unsigned int OrangutanServos::getServoSpeedB(unsigned char servoNum)
{
	if (servoNum >= numServosB)
		return 0;
	return servoSpeedB[servoNum & 7];
}


// stops timer 1, sets all servo outputs low, and frees up memory that's been used
// servos cannot be used after stop() is called without calling start() again.
void OrangutanServos::stop()
{
	// disable timer 1 interrupts and stop the timer
	TIMSK1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	
	unsigned char i;
	
	#ifndef _ORANGUTAN_SVP
	
	// set used servo pins as driving-low outputs
	for (i = 0; i < numServos; i++)
		*(portPin[i].portRegister) &= ~portPin[i].bitmask;
	
	#endif

	// set used servo pins as driving-low outputs
	for (i = 0; i < numServosB; i++)
		*(portPinB[i].portRegister) &= ~portPinB[i].bitmask;

	freeServoMemory();
}


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
