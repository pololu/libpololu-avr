/*
  OrangutanPushbuttons.h - Library for using the thee user pushbuttons on the
      Orangutan LV-168
  Written by Ben Schmidel, May 23, 2008.
  Released into the public domain.
*/

#ifndef F_CPU
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU

#include <avr/io.h>
#include <util/delay.h>
#include "include/OrangutanPushbuttons.h"


// constructor

OrangutanPushbuttons::OrangutanPushbuttons()
{

}

#ifdef LIB_ORANGUTAN

extern "C" unsigned char wait_for_button_press(unsigned char buttons)
{
  return OrangutanPushbuttons.waitForPress(buttons);
}

extern "C" unsigned char wait_for_button_release(unsigned char buttons)
{
  return OrangutanPushbuttons.waitForRelease(buttons);
}

extern "C" unsigned char wait_for_button(unsigned char buttons)
{
  return OrangutanPushbuttons.waitForButton(buttons);
}

extern "C" unsigned char button_is_pressed(unsigned char buttons)
{
  return OrangutanPushbuttons.isPressed(buttons);
}

#endif

// initializes I/O pins for use as button inputs
void OrangutanPushbuttons::init2()
{
	DDRB &= ~ALL_BUTTONS;	// set the pushbutton pins to be inputs
	PORTB |= ALL_BUTTONS;	// enable pullups on the pushbutton pins
	_delay_us(1);			// give pullups time to stabilize
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
		while (!(~PINB & buttons))	// wait for a button to be pressed
			;
		_delay_ms(10);				// debounce the button press
	}
	while (!(~PINB & buttons));		// if button isn't still pressed, loop
	return ~PINB & buttons;			// return the pressed button(s)
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
		while (!(PINB & buttons))	// wait for a button to be released
			;
		_delay_ms(10);				// debounce the button release
	}
	while (!(PINB & buttons));		// if button isn't still released, loop
	return PINB & buttons;			// return the released button(s)
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

// returns 1 (true) if any of the specified buttons is currently being
// pressed.  Otherwise this method returns 0 (false).  The argument
// 'buttons' can refer to multiple buttons using the bitwise OR operator |.
unsigned char OrangutanPushbuttons::isPressed(unsigned char buttons)
{
	init();		// initialize pushbutton I/O pins if necessary
	return (~PINB & buttons) ? 1 : 0;
}
