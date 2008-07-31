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

#ifdef LIB_POLOLU
#include "OrangutanTime.h"
#include <util/delay.h>


volatile unsigned long msCounter = 0;	// returned by millis()
unsigned int us_times_10 = 0;			// in units of 10^-7 s
unsigned char initialized = 0;

// this ISR is called every time timer2 overflows
ISR(TIMER2_OVF_vect)
{
#ifdef POLOLU_3PI
	us_times_10 += 255;
	if (us_times_10 >= 10000)
	{
		msCounter++;
		us_times_10 -= 10000;
	}
#else
	us_times_10 += 1024;
	if (us_times_10 >= 10000)
	{
		msCounter++;
		us_times_10 -= 10000;
	}
#endif
}


extern "C" void initTime()
{
	initialized = 1;
	TIMSK2 &= ~(1 << TOIE2);	// disable timer2 overflow interrupt

#ifdef POLOLU_3PI
	TCCR2A &= ~0x03;	// phase correct PWM, TOP = 0xFF
	TCCR2A |= 0x01;
	TCCR2B &= 0xF0;
	TCCR2B |= 0x01;		// timer2 ticks at 20 MHz (prescaler = 1)
#else
	TCCR2A |= 0x03;		// fast PWM, TOP = 0xFF
	TCCR2B &= 0xF0;
	TCCR2B |= 0x02;		// timer 2 ticks at 2.5 MHz (prescaler = 8)
#endif

	TIFR2 |= 1 << TOV2;	// clear timer2 overflow flag
	TIMSK2 |= 1 << TOIE2;	// enable timer2 overflow interrutp
	sei();				// enable global interrupts
}

extern "C" void delay(unsigned int milliseconds)
{
	while (milliseconds--)
	  _delay_ms(1);
}


extern "C" unsigned long millis()
{
	if (!initialized)
		initTime();
	return msCounter;
}

extern "C" unsigned long microseconds()
{
	if (!initialized)
		initTime();
	return msCounter * 1000 + us_time_10 / 10;
}
#endif // LIB_POLOLU
