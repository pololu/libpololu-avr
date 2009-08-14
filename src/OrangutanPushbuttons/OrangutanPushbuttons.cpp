/*
  OrangutanPushbuttons.cpp - Library for using the three user pushbuttons on the
      Orangutan LV, SV, SVP, or 3pi robot.
*/

/*
 * Written by Ben Schmidel, May 23, 2008.
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
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU

#include <avr/io.h>
#include "OrangutanPushbuttons.h"


// constructor

OrangutanPushbuttons::OrangutanPushbuttons()
{

}

#ifdef LIB_POLOLU

#include "../OrangutanTime/OrangutanTime.h"	// provides access to delay routines

extern "C" unsigned char wait_for_button_press(unsigned char buttons)
{
	return OrangutanPushbuttons::waitForPress(buttons);
}

extern "C" unsigned char wait_for_button_release(unsigned char buttons)
{
	return OrangutanPushbuttons::waitForRelease(buttons);
}

extern "C" unsigned char wait_for_button(unsigned char buttons)
{
	return OrangutanPushbuttons::waitForButton(buttons);
}

extern "C" unsigned char button_is_pressed(unsigned char buttons)
{
	return OrangutanPushbuttons::isPressed(buttons);
}

#else
#include "wiring.h"		// provides access to delay() and delayMicroseconds()
#endif

// initializes I/O pins for use as button inputs
void OrangutanPushbuttons::init2()
{
	BUTTON_DDR &= ~ALL_BUTTONS;		// set the pushbutton pins to be inputs
	BUTTON_PORT |= ALL_BUTTONS;		// enable pullups on the pushbutton pins
	delayMicroseconds(1);			// give pullups time to stabilize
}

// wait for any of the specified buttons to be pressed, at which point
// execution will return from this method.  The argument 'buttons' can
// refer to multiple buttons using the bitwise OR operator |.  The
// return value is the ID of the button that was pressed.  Note that
// this method takes care of button debouncing.
// Example: waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
unsigned char OrangutanPushbuttons::waitForPress(unsigned char buttons)
{
	init();		// initialize pushbutton I/O pins if necessary
	do
	{
		while (!(~BUTTON_PIN & buttons))	// wait for a button to be pressed
			;
			delay(10);						// debounce the button press
	}
	while (!(~BUTTON_PIN & buttons));		// if button isn't still pressed, loop
	return ~BUTTON_PIN & buttons;			// return the pressed button(s)
}


// wait for any of the specified buttons to be released, at which point
// execution will return from this method.  The argument 'buttons' can
// refer to multiple buttons using the bitwise OR operator |.  The
// return value is the ID of the button that was released.  Note that
// this method takes care of button debouncing.
// Example: waitForRelease(TOP_BUTTON | BOTTOM_BUTTON);
unsigned char OrangutanPushbuttons::waitForRelease(unsigned char buttons)
{
	init();		// initialize pushbutton I/O pins if necessary
	do
	{
		while (!(BUTTON_PIN & buttons))	// wait for a button to be released
			;
			delay(10);						// debounce the button release
	}
	while (!(BUTTON_PIN & buttons));		// if button isn't still released, loop
	return BUTTON_PIN & buttons;			// return the released button(s)
}

// wait for any of the specified buttons to be pressed and then released,
// at which point execution will return from this method.  The argument
// 'buttons' can refer to multiple buttons using the bitwise OR operator |.
// The return value is the ID of the button that was pressed and released.
// Note that this method takes care of button debouncing.
// Example: waitForButton(TOP_BUTTON | BOTTOM_BUTTON);
unsigned char OrangutanPushbuttons::waitForButton(unsigned char buttons)
{
  unsigned char pressedButton = waitForPress(buttons);
  waitForRelease(pressedButton);
  return pressedButton;
}

// returns the value of any specified buttons that are currently pressed, or 0.
// For example, if the top and middle buttons are both pressed, this method
// will return (TOP_BUTTON | MIDDLE_BUTTON).  The argument 'buttons' can
// refer to multiple buttons using the bitwise OR operator |.
unsigned char OrangutanPushbuttons::isPressed(unsigned char buttons)
{
	init();		// initialize pushbutton I/O pins if necessary
	return ~BUTTON_PIN & buttons;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
