/*
  OrangutanLEDs.cpp - Library for using the LED(s) on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
  Written by Ben Schmidel, May 27, 2008.
  Released into the public domain.
*/

#include <avr/io.h>
#include "OrangutanLEDs.h"

#define RED_LED		PD1
#define GREEN_LED	PD7

#ifdef LIB_POLOLU

extern "C" void left_led(unsigned char on)
{
	OrangutanLEDs::left(on);
}

extern "C" void right_led(unsigned char on)
{
	OrangutanLEDs::right(on);
}

#endif


// constructor
OrangutanLEDs::OrangutanLEDs()
{

}


// turns the Orangutan LED off if 'on' is zero, else this method
// turns the LED on.  Note that the Baby Orangutan B only has
// one LED (the red one), so green() will just drive I/O line PD7
// high or low, depending on the argument.
void OrangutanLEDs::left(unsigned char on)
{
	DDRD |= 1 << RED_LED;
	if (on)
		PORTD |= 1 << RED_LED;
	else
		PORTD &= ~(1 << RED_LED);
}


void OrangutanLEDs::right(unsigned char on)
{
	DDRD |= 1 << GREEN_LED;
	if (on)
		PORTD |= 1 << GREEN_LED;
	else
		PORTD &= ~(1 << GREEN_LED);
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
