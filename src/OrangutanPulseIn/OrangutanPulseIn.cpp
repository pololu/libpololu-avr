/*
  OrangutanPulseIn.cpp - Library for reading digital pulses with the
	Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  This library
	can be used to read RC inputs.  To use this library, you must
	repeatedly call the update() function in your main loop more often
	than your configured maximum pulse length.  If this is done,
	pulses longer than the maximum pulse length will be reported as having
	a duration of 0xFFFF (65535).  If update() is not called often enough,
	pulses longer than the maximum pulse length can be reported as having
	any pulse length from 0 and 65535, depending on how the timer has
	overflowed.
*/

/*
 * Written by Ben Schmidel, August 13, 2009.
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
 

#include "OrangutanPulseIn.h"
#include "../OrangutanDigital/OrangutanDigital.h"	// digital I/O routines
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


struct PulseInputStruct *pi;
unsigned char numPulsePins;


ISR(PCINT0_vect)
{
	unsigned int time = TCNT1;
	unsigned char i;
	for (i = 0; i < numPulsePins; i++)
	{
		unsigned char pr = *pi[i].pinRegister & pi[i].bitmask;
		if (pr ^ pi[i].inputState)
		{
			unsigned int width = time - pi[i].lastPCTime;
			if (width < pi[i].curPulseWidth)
			{
				width = 0xFFFF;
			}
			if (pi[i].inputState)
			{
				pi[i].lastHighPulse = width;
				pi[i].newPulse = HIGH_PULSE;
			}
			else
			{
				pi[i].lastLowPulse = width;
				pi[i].newPulse = LOW_PULSE;
			}
			
			pi[i].inputState = pr;
			pi[i].curPulseWidth = 0;
			pi[i].lastPCTime = time;
		}
	}
}

ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect,ISR_ALIASOF(PCINT0_vect));
#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__)
ISR(PCINT3_vect,ISR_ALIASOF(PCINT0_vect));
#endif



#ifdef LIB_POLOLU

extern "C" unsigned char init_pulse_in(const unsigned char *pulsePins, unsigned char numPins, unsigned char maxLengthEnum)
{
	return OrangutanPulseIn::init(pulsePins, numPins, maxLengthEnum);
}

extern "C" void update_pulse_in()
{
	OrangutanPulseIn::update();
}

extern "C" void set_max_pulse_length(unsigned char maxLengthEnum)
{
	OrangutanPulseIn::setMaxPulseLength(maxLengthEnum);
}

extern "C" struct PulseInputStruct get_pulse_info(unsigned char idx)
{
	return OrangutanPulseIn::getPulseInfo(idx);
}

#endif


// constructor
OrangutanPulseIn::OrangutanPulseIn()
{

}


void freeMemory()
{
	if (pi != 0)
	{
		free(pi);
		pi = 0;
	}
}


// the destructor frees up allocated memory
OrangutanPulseIn::~OrangutanPulseIn()
{
	freeMemory();
}


unsigned char OrangutanPulseIn::init(const unsigned char *pulsePins, unsigned char numPins, unsigned char maxLengthEnum)
{
	TIMSK1 = 0;			// disable all timer 1 interrupts
	TCCR1A = 0;

	if (maxLengthEnum < 1)
		maxLengthEnum = 1;
	if (maxLengthEnum > 5)
		maxLengthEnum = 5;
	TCCR1B = maxLengthEnum;	// set clock tick rate (determines resolution and max measurable pulse length)
	
	PCICR = 0;			// disable pin-change interrupts
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__)
	PCMSK3 = 0;
#endif

	numPulsePins = numPins;

	freeMemory();
	pi = (struct PulseInputStruct*)malloc(sizeof(struct PulseInputStruct)*numPins);
	if (pi == 0)
		return 1;
		
	unsigned char i;
	struct IOStruct io;
	for (i = 0; i < numPins; i++)
	{
		OrangutanDigital::getIORegisters(&io, pulsePins[i]);
		OrangutanDigital::setDataDirection(&io, 0);			// set pin as an input
		OrangutanDigital::setOutputValue(&io, 0);				// internal pull-up disabled
		pi[i].pinRegister = io.pinRegister;
		pi[i].bitmask = io.bitmask;
		pi[i].lastHighPulse = 0;
		pi[i].lastLowPulse = 0;
		pi[i].curPulseWidth = 0xFFFF;
		pi[i].inputState = *io.pinRegister & io.bitmask;
		pi[i].newPulse = 0;
		
#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__)

		if (io.pinRegister == &PINA)
			PCMSK0 |= io.bitmask;
		if (io.pinRegister == &PINB)
			PCMSK1 |= io.bitmask;
		if (io.pinRegister == &PINC)
			PCMSK2 |= io.bitmask;
		if (io.pinRegister == &PIND)
			PCMSK3 |= io.bitmask;

#else

		if (io.pinRegister == &PINB)
			PCMSK0 |= io.bitmask;
		if (io.pinRegister == &PINC)
			PCMSK1 |= io.bitmask;
		if (io.pinRegister == &PIND)
			PCMSK2 |= io.bitmask;

#endif
	}
	
	PCIFR = 0XFF;		// cancel any pending pin-change interrupts
	if (PCMSK0)
		PCICR |= PCIE0;
	if (PCMSK1)
		PCICR |= PCIE1;
	if (PCMSK2)
		PCICR |= PCIE2;
#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__)
	if (PCMSK3)
		PCICR |= PCIE3;
#endif

	sei();
	
	return 0;
}


void OrangutanPulseIn::update()
{
	unsigned char i;
	for (i = 0; i < numPulsePins; i++)
	{
		unsigned char origPCICR = PCICR;
		PCICR = 0;				// disable pin-change interrupts
		unsigned int width = TCNT1 - pi[i].lastPCTime;
		if (width < pi[i].curPulseWidth)
		{
			width = 0xFFFF;
		}
		pi[i].curPulseWidth = width;
		PCICR = origPCICR;		// re-enable pin-change interrupts
	}
}


void OrangutanPulseIn::setMaxPulseLength(unsigned char maxLengthEnum)
{
	unsigned char origPCICR = PCICR;
	PCICR = 0;				// disable pin-change interrupts
	if (maxLengthEnum < 1)
		maxLengthEnum = 1;
	if (maxLengthEnum > 5)
		maxLengthEnum = 5;
	TCCR1B = maxLengthEnum;

	unsigned char i;
	for (i = 0; i < numPulsePins; i++)
	{
		pi[i].lastHighPulse = 0;
		pi[i].lastLowPulse = 0;
		pi[i].curPulseWidth = 0xFFFF;
		pi[i].inputState = *pi[i].pinRegister & pi[i].bitmask;
		pi[i].newPulse = 0;
	}
	PCIFR = 0XFF;			// cancel any pending pin-change interrupts
	PCICR = origPCICR;		// re-enable pin-change interrupts
}


struct PulseInputStruct OrangutanPulseIn::getPulseInfo(unsigned char idx)
{
	struct PulseInputStruct ret;
	
	if (idx >= numPulsePins)
		return ret;
		
	unsigned char origPCICR = PCICR;
	PCICR = 0;				// disable pin-change interrupts
	
	ret = pi[idx];
	
	PCICR = origPCICR;		// re-enable pin-change interrupts
	
	return ret;
}


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
