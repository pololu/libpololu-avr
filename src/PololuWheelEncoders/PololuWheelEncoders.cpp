/*
  PololuWheelEncoders.h - Library for using Pololu Wheel Encoders.
*/
	
/*
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <avr/io.h>
#include <stdlib.h>
#include "PololuWheelEncoders.h"

#ifdef LIB_POLOLU

extern "C" void encoders_init(unsigned char a1, unsigned char a2, unsigned char b1, unsigned char b2)
{
	PololuWheelEncoders::init(a1,a2,b1,b2);
}

extern "C" int get_encoder_counts_a()
{
	return PololuWheelEncoders::getCountsA();
}

extern "C" int get_encoder_counts_b()
{
	return PololuWheelEncoders::getCountsB();
}

extern "C" int get_encoder_counts_and_reset_a()
{
	return PololuWheelEncoders::getCountsAndResetA();
}

extern "C" int get_encoder_counts_and_reset_b()
{
	return PololuWheelEncoders::getCountsAndResetB();
}

#endif

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

static char global_a1;
static char global_a2;
static char global_b1;
static char global_b2;

static char global_counts_a;
static char global_counts_b;

static char global_error_a;
static char global_error_b;

static char global_last_a1_val;
static char global_last_a2_val;
static char global_last_b1_val;
static char global_last_b2_val;

inline unsigned char get_val(unsigned char pin)
{
	if(pin <= 7)
		return (PIND >> pin) & 1;
	if(pin <= 13)
		return (PINB >> (pin-8)) & 1;
	return (PINC >> (pin-14)) & 1;
}

ISR(PCINT0_vect)
{
	unsigned char a1_val = get_val(global_a1);
	unsigned char a2_val = get_val(global_a2);
	unsigned char b1_val = get_val(global_b1);
	unsigned char b2_val = get_val(global_b2);

	char plus_a = a1_val ^ global_last_a2_val;
	char minus_a = a2_val ^ global_last_a1_val;
	char plus_b = b1_val ^ global_last_b2_val;
	char minus_b = b2_val ^ global_last_b1_val;

	if(plus_a)
		global_counts_a += 1;
	if(minus_a)
		global_counts_a -= 1;

	if(plus_b)
		global_counts_b += 1;
	if(minus_b)
		global_counts_b -= 1;

	if(a1_val != global_last_a1_val && b1_val != global_last_b1_val)
		global_error_a = 1;
	if(a1_val != global_last_a1_val && b1_val != global_last_b1_val)
		global_error_b = 1;

	global_last_a1_val = a1_val;
	global_last_a2_val = a2_val;
	global_last_b1_val = b1_val;
	global_last_b2_val = b2_val;
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
}

void PololuWheelEncoders::init(unsigned char a1, unsigned char a2, unsigned char b1, unsigned char b2)
{
	global_a1 = a1;
	global_a2 = a2;
	global_b1 = b1;
	global_b2 = b2;

	enable_interrupts_for_pin(a1);
	enable_interrupts_for_pin(a2);
	enable_interrupts_for_pin(b1);
	enable_interrupts_for_pin(b2);

	// initialize the global state
	global_counts_a = 0;
	global_counts_b = 0;
	global_error_a = 0;
	global_error_b = 0;

	global_last_a1_val = get_val(global_a1);
	global_last_a2_val = get_val(global_a2);
	global_last_b1_val = get_val(global_b1);
	global_last_b2_val = get_val(global_b2);
}

int PololuWheelEncoders::getCountsA() {	return global_counts_a; }
int PololuWheelEncoders::getCountsB() {	return global_counts_a; }

int PololuWheelEncoders::getCountsAndResetA()
{
	int tmp = global_counts_a;
	global_counts_a = 0;
	return tmp;
}

int PololuWheelEncoders::getCountsAndResetB()
{
	int tmp = global_counts_b;
	global_counts_b = 0;
	return tmp;
}

unsigned char PololuWheelEncoders::checkErrorA()
{
	unsigned char tmp = global_error_a;
	global_error_a = 0;
	return tmp;
}

unsigned char PololuWheelEncoders::checkErrorB()
{
	unsigned char tmp = global_error_b;
	global_error_b = 0;
	return tmp;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
