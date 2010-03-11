/*
  OrangutanTime.cpp - Library for timing routines, calibrated for Orangutan, which runs at 20 MHz.
  This should *NOT* be used as an Arduino library.
*/

/*
 * Written by Ben Schmidel et al., July 30, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

// Define a macro so that OrangutanTime.h can exclude definitions that
// conflict with the C-only functions defined here.
#define OrangutanTime_cpp

#include "OrangutanTime.h"
#include <avr/interrupt.h>

volatile unsigned long msCounter = 0;	// returned by millis()
unsigned int us_over_10 = 0;				// in units of 10^-7 s (intentionally not volatile)

extern "C" void TIMER2_OVF_vect() __attribute__((naked, __INTR_ATTRS));
extern "C" void TIMER2_OVF_vect()
{
	__asm__ volatile (
		//save values of registers we need to use here
		"push r2"					"\n\t"	// SREG holder
		"push r24"					"\n\t"	// save the registers we will use on the stack
		"push r25"					"\n\t"	//  so that we can restore them at the end
		"in   r2, 0x3f"				"\n\t"	// 0x3f is SREG

		"lds  r25, us_over_10+1"	"\n\t"	// load high byte of us_over_10 from RAM
		"subi r25, 252"				"\n\t"	// add 4 to the high byte (i.e. us_over_10 + 1024)
		"sts  us_over_10+1, r25"	"\n\t"	// save the new value to us_over_10 (in RAM)
		"lds  r24, us_over_10"		"\n\t"	// load the low byte of us_over_10

		"subi r24, 0x10"			"\n\t"	// with the following line, subtract 10000
		"sbci r25, 0x27"			"\n\t"	//  from r25:r24 (i.e. us_over_10 - 10000)
		"brcs end"					"\n\t"	// branch to "end" if carry flag is set
											// (i.e. if (us_over_10 < 10000) goto end)
		"sts  us_over_10+1, r25"	"\n\t"	// with the following line,
		"sts  us_over_10, r24"		"\n\t"	//  us_over_10 = us_over_10 - 10000

		"lds  r24, msCounter"		"\n\t"	// load lowest byte of msCounter from RAM
		"lds  r25, msCounter+1"		"\n\t"	// load next lowest byte of msCounter from RAM
		"adiw r24, 1"				"\n\t"	// add 1 to the word r24:r25
		"sts  msCounter, r24"		"\n\t"	// save byte to RAM
		"sts  msCounter+1, r25"		"\n\t"	// save byte to RAM
		"ldi  r25, 0"				"\n\t"	// load 0 into r25
		"lds  r24, msCounter+2"		"\n\t"	// load 3rd byte of msCounter from RAM
		"adc  r24, r25"				"\n\t"	// add carry from previous additon operation
		"sts  msCounter+2, r24"		"\n\t"	// save the byte to RAM
		"lds  r24, msCounter+3"		"\n\t"	// load highest byte of msCounter from RAM
		"adc  r24, r25"				"\n\t"	// carry from previous addition operation
		"sts  msCounter+3, r24"		"\n\t"	// save the byte to RAM

		"end: out 0x3f, r2"			"\n\t"	// restore SREG
		"pop r25"					"\n\t"	// restore the registers we used in this ISR
		"pop r24"					"\n\t"
		"pop r2"					"\n\t"

		"reti"						"\n\t"	// return
		);
}

// Standard aliases for the static class functions, accessible from C.
// There are some inline ones defined to alias to these in the C
// header files.  Also, delay_us() is only in the header, since it's
// inline assembly code.

extern "C" {
	unsigned long get_ms() { return OrangutanTime::ms(); }
	void delay_ms(unsigned int milliseconds) { OrangutanTime::delayMilliseconds(milliseconds); }

	void time_reset() { OrangutanTime::reset(); }
}

unsigned long OrangutanTime::ms()
{
	init();
	unsigned long value;
	TIMSK2 &= ~(1 << TOIE2);	// disable timer2 overflow interrupt
	value = msCounter;
	TIMSK2 |= 1 << TOIE2;	// enable timer2 overflow interrupt
	return value;
}

void OrangutanTime::delayMilliseconds(unsigned int milliseconds)
{
	while (milliseconds--)
	  delayMicroseconds(1000);
}

void OrangutanTime::init2()
{
	TIMSK2 &= ~(1 << TOIE2);	// disable timer2 overflow interrupt

/*  40 kHz operation
		us_over_10_increment = 255;	// increment us_over_10 by this every timer 2 overflow
		TCCR2A &= ~0x03;	// phase correct PWM, TOP = 0xFF
		TCCR2A |= 0x01;
		TCCR2B &= 0xF0;
		TCCR2B |= 0x01;		// timer2 ticks at 20 MHz (prescaler = 1)
*/

	TCCR2A |= 0x03;		// fast PWM, TOP = 0xFF
	TCCR2B &= 0xF0;
	TCCR2B |= 0x02;		// timer 2 ticks at 2.5 MHz (prescaler = 8)

	TIFR2 |= 1 << TOV2;	// clear timer2 overflow flag
	TIMSK2 |= 1 << TOIE2;	// enable timer2 overflow interrupt
	sei();				// enable global interrupts
}

void OrangutanTime::reset()
{
	init();
	TIMSK2 &= ~(1 << TOIE2);	// disable timer2 overflow interrupt
	msCounter = 0;
	us_over_10 = 0;
	TIFR2 |= 1 << TOV2;	// clear timer2 overflow flag
	TIMSK2 |= 1 << TOIE2;	// enable timer2 overflow interrupt
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
