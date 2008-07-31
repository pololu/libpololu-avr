/*
  OrangutanDelay.cpp - Library for delay, calibrated for Orangutan.  This
  should *NOT* be used as an Arduino library.
*/

/*
 * Written by Ben Schmidel et al., July 30, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18/2
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

// Define a macro so that OrangutanTime.h can exclude definitions that
// conflict with the C-only functions defined here.
#define OrangutanTime_cpp

#include "OrangutanTime.h"
#include <avr/interrupt.h>

volatile unsigned long msCounter = 0;	// returned by millis()
unsigned int us_over_10 = 0;			// in units of 10^-7 s

// this ISR is called every time timer2 overflows
// This is exactly every 25.5 us for 40khz pwm, 102.4 us for 10khz.
unsigned int us_over_10_increment = 0; // either 255 or 1024

ISR(TIMER2_OVF_vect)
{
	us_over_10 += us_over_10_increment;

	if (us_over_10 >= 10000)
	{
		msCounter++;
		us_over_10 -= 10000;
	}
}

// Standard aliases for the static class functions, accessible from C.
// There are some inline ones defined to alias to these in the C
// header files.  Also, delay_us() is only in the header, since it's
// inline assembly code.

extern "C" {
	unsigned long get_ms() { return OrangutanTime::ms(); }
	unsigned long get_us() { return OrangutanTime::us(); }
	void delay_ms(unsigned int milliseconds) { OrangutanTime::delayMilliseconds(milliseconds); }

	void time_init(char use_40khz) { OrangutanTime::init(use_40khz); }
	void time_reset() { OrangutanTime::reset(); }
}

unsigned long OrangutanTime::ms()
{
	return msCounter;
}

unsigned long OrangutanTime::us()
{
	return msCounter * 1000 + us_over_10 / 10;
}

void OrangutanTime::delayMilliseconds(unsigned int milliseconds)
{
	while (milliseconds--)
	  delayMicroseconds(1000);
}

void OrangutanTime::init(char use_40khz)
{
	TIMSK2 &= ~(1 << TOIE2);	// disable timer2 overflow interrupt

	if(use_40khz)
	{
		us_over_10_increment = 255;
		TCCR2A &= ~0x03;	// phase correct PWM, TOP = 0xFF
		TCCR2A |= 0x01;
		TCCR2B &= 0xF0;
		TCCR2B |= 0x01;		// timer2 ticks at 20 MHz (prescaler = 1)
	}
	else
	{
		us_over_10_increment = 1024;
		TCCR2A |= 0x03;		// fast PWM, TOP = 0xFF
		TCCR2B &= 0xF0;
		TCCR2B |= 0x02;		// timer 2 ticks at 2.5 MHz (prescaler = 8)
	}

	TIFR2 |= 1 << TOV2;	// clear timer2 overflow flag
	TIMSK2 |= 1 << TOIE2;	// enable timer2 overflow interrutp
	sei();				// enable global interrupts
}

void OrangutanTime::reset()
{
	msCounter = 0;
	us_over_10 = 0;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
