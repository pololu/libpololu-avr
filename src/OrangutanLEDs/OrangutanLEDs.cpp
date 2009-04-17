/*
  OrangutanLEDs.cpp - Library for using the LED(s) on the
      Orangutan LV-168, Baby Orangutan B-48/B-168, or 3pi robot.
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18/6
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
#include "OrangutanLEDs.h"


#define RED_LED		PORT1
#define GREEN_LED	PORT7

#ifdef LIB_POLOLU

extern "C" void left_led(unsigned char on)
{
	OrangutanLEDs::left(on);
}

extern "C" void right_led(unsigned char on)
{
	OrangutanLEDs::right(on);
}

extern "C" void red_led(unsigned char on)
{
	OrangutanLEDs::red(on);
}

extern "C" void green_led(unsigned char on)
{
	OrangutanLEDs::green(on);
}

#endif


// constructor
OrangutanLEDs::OrangutanLEDs()
{

}


// turns the target LED off if 'on' is zero, else this method
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
