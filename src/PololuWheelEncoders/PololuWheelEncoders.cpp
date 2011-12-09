/*
  PololuWheelEncoders.cpp - Library for using Pololu Wheel Encoders.
*/
	
/*
 * Copyright (c) 2009-2010 Pololu Corporation. For more information, see
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


#include "../OrangutanLEDs/OrangutanLEDs.h"

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

inline unsigned char get_val(unsigned char pin)
{
	// Note: get_val will work (i.e. always return the same value)
	// even with invalid pin values, since the bit shift on the final
	// return will cause the port value to be shifted all the way to
	// 0.
	if(pin <= 7)
		return (PIND >> pin) & 1;
	if(pin <= 13)
		return (PINB >> (pin-8)) & 1;
	return (PINC >> (pin-14)) & 1;
}

ISR(PCINT0_vect)
{
	unsigned char m1a_val = get_val(global_m1a);
	unsigned char m2a_val = get_val(global_m2a);
	unsigned char m1b_val = get_val(global_m1b);
	unsigned char m2b_val = get_val(global_m2b);

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

static void enable_interrupts_for_pin(unsigned char p)
{
	// check what block it's in and do the right thing
	if(p <= 7)
	{
		PCICR |= 1 << PCIE2;
		DDRD &= ~(1 << p);
		PCMSK2 |= 1 << p;
	}
	else if(p <= 13)
	{
		PCICR |= 1 << PCIE0;
		DDRB &= ~(1 << (p - 8));
		PCMSK0 |= 1 << (p - 8);
	}
	else if(p <= 19)
	{
		PCICR |= 1 << PCIE1;
		DDRC &= ~(1 << (p - 14));
		PCMSK1 |= 1 << (p - 14);
	}
	// Note: this will work with invalid port numbers, since there is
	// no final "else" clause.
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

	global_last_m1a_val = get_val(global_m1a);
	global_last_m1b_val = get_val(global_m1b);
	global_last_m2a_val = get_val(global_m2a);
	global_last_m2b_val = get_val(global_m2b);

	// Clear the interrupt flags in case they were set before for any reason.
	// On the AVR, interrupt flags are cleared by writing a logical 1
	// to them.
	PCIFR |= (1 << PCIF0) | (1 << PCIF1) | (1 << PCIF2);

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
