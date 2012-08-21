/*
  PololuWheelEncoders.cpp - Library for using Pololu Wheel Encoders.
*/
	
/*
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "PololuWheelEncoders.h"
#include "../OrangutanDigital/OrangutanDigital.h"       // digital I/O routines
#include "../OrangutanResources/include/OrangutanModel.h"


extern "C" void encoders_init(unsigned char m1a, unsigned char m1b, unsigned char m2a, unsigned char m2b)
{
	PololuWheelEncoders::init(m1a,m1b,m2a,m2b);
}

extern "C" int encoders_get_counts_m1()
{
	return PololuWheelEncoders::getCountsM1();
}

extern "C" int encoders_get_counts_m2()
{
	return PololuWheelEncoders::getCountsM2();
}

extern "C" int encoders_get_counts_and_reset_m1()
{
	return PololuWheelEncoders::getCountsAndResetM1();
}

extern "C" int encoders_get_counts_and_reset_m2()
{
	return PololuWheelEncoders::getCountsAndResetM2();
}

extern "C" int encoders_check_error_m1()
{
	return PololuWheelEncoders::checkErrorM1();
}

extern "C" int encoders_check_error_m2()
{
	return PololuWheelEncoders::checkErrorM2();
}


/*
 * Pin Change interrupts
 * PCI0 triggers on PCINT7..0
 * PCI1 triggers on PCINT14..8
 * PCI2 triggers on PCINT23..16
 * PCMSK2, PCMSK1, PCMSK0 registers control which pins contribute.
 *
 * The following table is useful:
 *
 * Arduino pin  AVR pin    PCINT #            PCI #
 * -----------  ---------  -----------------  -----
 * 0 - 7        PD0 - PD7  PCINT16 - PCINT23  PCI2
 * 8 - 13       PB0 - PB5  PCINT0 - PCINT5    PCI0
 * 14 - 19      PC0 - PC5  PCINT8 - PCINT13   PCI1
 *
 */

static char global_m1a;
static char global_m2a;
static char global_m1b;
static char global_m2b;

static int global_counts_m1;
static int global_counts_m2;

static char global_error_m1;
static char global_error_m2;

static char global_last_m1a_val;
static char global_last_m2a_val;
static char global_last_m1b_val;
static char global_last_m2b_val;

ISR(PCINT0_vect)
{
	unsigned char m1a_val = OrangutanDigital::isInputHigh(global_m1a);
	unsigned char m2a_val = OrangutanDigital::isInputHigh(global_m2a);
	unsigned char m1b_val = OrangutanDigital::isInputHigh(global_m1b);
	unsigned char m2b_val = OrangutanDigital::isInputHigh(global_m2b);

	char plus_m1 = m1a_val ^ global_last_m1b_val;
	char minus_m1 = m1b_val ^ global_last_m1a_val;
	char plus_m2 = m2a_val ^ global_last_m2b_val;
	char minus_m2 = m2b_val ^ global_last_m2a_val;

	if(plus_m1)
		global_counts_m1 += 1;
	if(minus_m1)
		global_counts_m1 -= 1;

	if(plus_m2)
		global_counts_m2 += 1;
	if(minus_m2)
		global_counts_m2 -= 1;

	if(m1a_val != global_last_m1a_val && m1b_val != global_last_m1b_val)
		global_error_m1 = 1;
	if(m2a_val != global_last_m2a_val && m2b_val != global_last_m2b_val)
		global_error_m2 = 1;

	global_last_m1a_val = m1a_val;
	global_last_m1b_val = m1b_val;
	global_last_m2a_val = m2a_val;
	global_last_m2b_val = m2b_val;
}

ISR(PCINT1_vect,ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect,ISR_ALIASOF(PCINT0_vect));
#ifdef PCINT3_vect
ISR(PCINT3_vect,ISR_ALIASOF(PCINT0_vect));
#endif

static void enable_interrupts_for_pin(unsigned char p)
{
	// TODO: Unify this with the code in OrangutanPulseIn::start
	// that does the same thing, and move it to OrangutanDigital.

	struct IOStruct io;
	OrangutanDigital::getIORegisters(&io, p);

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

	// Preserving the old behavior of the library prior to 2012-08-21,
	// we make the line be an input but do not specify whether its pull-up
	// should be enabled or not.
	*io.ddrRegister &= ~io.bitmask;

	// For simplicity set all the bits in PCICR and let the enabling of
	// pin-change interrupts be solely controlled by PCMSKx bits.
	PCICR = 0xFF;
}

void PololuWheelEncoders::init(unsigned char m1a, unsigned char m1b, unsigned char m2a, unsigned char m2b)
{
	global_m1a = m1a;
	global_m1b = m1b;
	global_m2a = m2a;
	global_m2b = m2b;

	// disable interrupts while initializing
	cli();

	enable_interrupts_for_pin(m1a);
	enable_interrupts_for_pin(m1b);
	enable_interrupts_for_pin(m2a);
	enable_interrupts_for_pin(m2b);

	// initialize the global state
	global_counts_m1 = 0;
	global_counts_m2 = 0;
	global_error_m1 = 0;
	global_error_m2 = 0;

	global_last_m1a_val = OrangutanDigital::isInputHigh(global_m1a);
	global_last_m1b_val = OrangutanDigital::isInputHigh(global_m1b);
	global_last_m2a_val = OrangutanDigital::isInputHigh(global_m2a);
	global_last_m2b_val = OrangutanDigital::isInputHigh(global_m2b);

	// Clear the interrupt flags in case they were set before for any reason.
	// On the AVR, interrupt flags are cleared by writing a logical 1
	// to them.
	PCIFR = 0xFF;

	// enable interrupts
	sei();
}

int PololuWheelEncoders::getCountsM1()
{
	cli();
	int tmp = global_counts_m1;
	sei();
	return tmp;
}

int PololuWheelEncoders::getCountsM2()
{
	cli();
	int tmp = global_counts_m2;
	sei();
	return tmp;
}

int PololuWheelEncoders::getCountsAndResetM1()
{
	cli();
	int tmp = global_counts_m1;
	global_counts_m1 = 0;
	sei();
	return tmp;
}

int PololuWheelEncoders::getCountsAndResetM2()
{
	cli();
	int tmp = global_counts_m2;
	global_counts_m2 = 0;
	sei();
	return tmp;
}

unsigned char PololuWheelEncoders::checkErrorM1()
{
	unsigned char tmp = global_error_m1;
	global_error_m1 = 0;
	return tmp;
}

unsigned char PololuWheelEncoders::checkErrorM2()
{
	unsigned char tmp = global_error_m2;
	global_error_m2 = 0;
	return tmp;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
