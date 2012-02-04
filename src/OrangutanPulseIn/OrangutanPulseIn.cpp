/*
  OrangutanPulseIn.cpp - Library for reading digital pulses with the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  Pulses
	are measured in ticks, which have units of 0.4 us.  Note that the
	pulse info is updated by pin change interrupts as the pulses occur,
	so unless you use the get_pulse_info() function to freeze a snapshot
	of the current pulse state, values might change on you in unexpected
	ways.  For example, if you want to do something only when there is
	a new high pulse, you might do the following:
	
	if (new_high_pulse(0) && !new_low_pulse(0))
		do something with pulse data
		
	However, a pulse might occur right after you make it past the if
	statement but before you run your conditional code, changing the
	condition to one that would never have made it past the if statement.
*/

/*
 * Written by Ben Schmidel, August 13, 2009.
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
 

#include "../OrangutanResources/include/OrangutanModel.h"
#include "OrangutanPulseIn.h"
#include "../OrangutanDigital/OrangutanDigital.h"	// digital I/O routines
#include "../OrangutanTime/OrangutanTime.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


struct PulseInputStruct *pis;
unsigned char numPulsePins;

extern volatile unsigned long tickCount;

ISR(PCINT0_vect)
{
	// the following is copied from OrangutanTime::ticks() since this is faster than calling
	// the ticks() method:
	unsigned long time = TCNT2 | tickCount;
	if (TIFR2 & (1 << TOV2))	// if TCNT2 has overflowed since we disabled t2 ovf interrupt
	{
		// NOTE: it is important to perform this computation again.  If we use a value of TCNT2 read
		// before we checked for the overflow, it might be something like 255 while it becomes 0 after
		// the overflow.  Using an old value could produce a result that is bigger than it should be.
		// For example, the following line should *NOT* be: numTicks += 256;
		time = TCNT2 | (tickCount + 256);		// compute ticks again and add 256 for the overflow
	}
	
	unsigned char i;
	for (i = 0; i < numPulsePins; i++)
	{
		unsigned char pr = (*pis[i].pinRegister & pis[i].bitmask) != 0;
		if (pr != pis[i].inputState)
		{
			unsigned long width = time - pis[i].lastPCTime;

			if (pis[i].inputState)
			{
				pis[i].lastHighPulse = width;
				pis[i].newPulse |= HIGH_PULSE;
			}
			else
			{
				pis[i].lastLowPulse = width;
				pis[i].newPulse |= LOW_PULSE;
			}
			
			pis[i].inputState = pr;
			pis[i].lastPCTime = time;
		}
	}
}

ISR(PCINT1_vect,ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect,ISR_ALIASOF(PCINT0_vect));
#ifdef PCINT3_vect  // this ISR only available on the Orangutan SVP and X2
ISR(PCINT3_vect,ISR_ALIASOF(PCINT0_vect));
#endif


// use of pulse_in_init() is discouraged; use pulse_in_start() instead
extern "C" unsigned char pulse_in_start(const unsigned char *pulsePins, unsigned char numPins)
{
	return OrangutanPulseIn::start(pulsePins, numPins);
}

extern "C" void get_pulse_info(unsigned char idx, struct PulseInputStruct* pulse_info)
{
	OrangutanPulseIn::getPulseInfo(idx, pulse_info);
}

extern "C" unsigned char new_pulse(unsigned char idx)
{
	return OrangutanPulseIn::newPulse(idx);
}

extern "C" unsigned char new_high_pulse(unsigned char idx)
{
	return OrangutanPulseIn::newHighPulse(idx);
}

extern "C" unsigned char new_low_pulse(unsigned char idx)
{
	return OrangutanPulseIn::newLowPulse(idx);
}

extern "C" unsigned long get_last_high_pulse(unsigned char idx)
{
	return OrangutanPulseIn::getLastHighPulse(idx);
}

extern "C" unsigned long get_last_low_pulse(unsigned char idx)
{
	return OrangutanPulseIn::getLastLowPulse(idx);
}

extern "C" void get_current_pulse_state(unsigned char idx, unsigned long* pulse_width, unsigned char* state)
{
	OrangutanPulseIn::getCurrentState(idx, pulse_width, state);
}

extern "C" unsigned long pulse_to_microseconds(unsigned long pulse)
{
	return OrangutanPulseIn::toMicroseconds(pulse);
}

extern "C" void pulse_in_stop()
{
	OrangutanPulseIn::stop();
}


// constructor
OrangutanPulseIn::OrangutanPulseIn()
{

}


void freePulseMemory()
{
	if (pis != 0)
	{
		free(pis);
		pis = 0;
	}
}


// the destructor frees up allocated memory
OrangutanPulseIn::~OrangutanPulseIn()
{
	freePulseMemory();
}


// Note: the initialization function deliberately does not set the specified pins as inputs
unsigned char OrangutanPulseIn::start(const unsigned char *pulsePins, unsigned char numPins)
{
	PCICR = 0;			// disable pin-change interrupts
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
    #ifdef PCMSK3  // for the Orangutan X2 and SVP
	PCMSK3 = 0;
    #endif

	numPulsePins = numPins;

	freePulseMemory();
	pis = (struct PulseInputStruct*)malloc(sizeof(struct PulseInputStruct)*numPins);
	if (pis == 0)
		return 1;
		
	unsigned char i;
	struct IOStruct io;
	for (i = 0; i < numPins; i++)
	{
		OrangutanDigital::getIORegisters(&io, pulsePins[i]);
		pis[i].pinRegister = io.pinRegister;
		pis[i].bitmask = io.bitmask;
		pis[i].lastHighPulse = 0;
		pis[i].lastLowPulse = 0;
		pis[i].lastPCTime = OrangutanTime::ticks();
		pis[i].inputState = *io.pinRegister & io.bitmask;
		pis[i].newPulse = 0;
		
#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)

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
		PCICR |= 1 << PCIE0;
	if (PCMSK1)
		PCICR |= 1 << PCIE1;
	if (PCMSK2)
		PCICR |= 1 << PCIE2;
#ifdef PCMSK3  // for the Orangutan X2 and SVP
	if (PCMSK3)
		PCICR |= 1 << PCIE3;
#endif

	sei();
	
	return 0;
}


// Places a snapshot of the current pulse state into the supplied pulseInfo argument.
// This lets the user evaluate the state without worrying that the data will change
// in the middle of the analysis due to an incoming pulse.
void OrangutanPulseIn::getPulseInfo(unsigned char idx, struct PulseInputStruct* pulseInfo)
{
	if (idx >= numPulsePins)
		return;
		
	unsigned char origPCICR = PCICR;
	PCICR = 0;				// disable pin-change interrupts
	
	*pulseInfo = pis[idx];
	pis[idx].newPulse = 0;
	
	PCICR = origPCICR;		// re-enable pin-change interrupts
}


// returns pulse state if there is a new pulse on the specified channel of the specified state
// this is a PRIVATE method
unsigned char OrangutanPulseIn::newPulse(unsigned char idx, unsigned char state)
{
	if (idx >= numPulsePins)
		return 0;

	unsigned char val = 0;

	if (pis[idx].newPulse & state)
	{
		val = pis[idx].newPulse;
		unsigned char origPCICR = PCICR;
		PCICR = 0;				// disable pin-change interrupts
		pis[idx].newPulse &= ~state;
		PCICR = origPCICR;		// re-enable pin-change interrupts
	}
	
	return val;
}


unsigned long OrangutanPulseIn::getLastHighPulse(unsigned char idx)
{
	unsigned long val = 0;

	// make sure we get a good reading of the last high pulse
	// without disabling interrupts
	do
	{
		val = pis[idx].lastHighPulse;
	}
	while (val != pis[idx].lastHighPulse);
	
	return val;
}

unsigned long OrangutanPulseIn::getLastLowPulse(unsigned char idx)
{
	unsigned long val = 0;

	// make sure we get a good reading of the last high pulse
	// without disabling interrupts
	do
	{
		val = pis[idx].lastLowPulse;
	}
	while (val != pis[idx].lastLowPulse);
	
	return val;
}



void OrangutanPulseIn::getCurrentState(unsigned char idx, unsigned long* pulseWidth, unsigned char *state)
{
	// make sure we get a good reading of the current pulse width
	// without disabling interrupts
	do
	{
		*state = pis[idx].inputState;
		*pulseWidth = pis[idx].lastPCTime;
	}
	while (*pulseWidth != pis[idx].lastPCTime);
	*pulseWidth = OrangutanTime::ticks() - *pulseWidth;
}



// Disables pin change interrupts and frees memory that's been used
// After calling stop(), start() must be recalled to begin reading
// pulses again.
void OrangutanPulseIn::stop()
{
	PCICR = 0;			// disable pin-change interrupts
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
    #ifdef PCMSK3  // for the Orangutan X2 and SVP
	PCMSK3 = 0;
    #endif

	freePulseMemory();
}




// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
