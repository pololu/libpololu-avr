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
#include "OrangutanPushbuttons.h"


// constructor

OrangutanPushbuttons::OrangutanPushbuttons()
{

}


// initializes I/O pins for use as button inputs
void OrangutanPushbuttons::init()
{
	DDRB &= ~ALL_BUTTONS;	// set the pushbutton pins to be inputs
	PORTB |= ALL_BUTTONS;	// enable pullups on the pushbutton pins
}


// wait for any of the specified buttons to be pressed, at which point
// execution will return from this method.  The argument 'buttons' can
// refer to multiple buttons using the bitwise OR operator |.  The
// return value is the ID of the button that was pressed.  Note that
// this method takes care of button debouncing.
// Example: waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
uint8_t OrangutanPushbuttons::waitForPress(uint8_t buttons)
{
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
uint8_t OrangutanPushbuttons::waitForRelease(uint8_t buttons)
{
	do
	{
		while (!(PINB & buttons))	// wait for a button to be released
			;
		_delay_ms(10);				// debounce the button release
	}
	while (!(PINB & buttons));		// if button isn't still released, loop
	return PINB & buttons;			// return the released button(s)
}


// returns 1 (true) if any of the specified buttons is currently being
// pressed.  Otherwise this method returns 0 (false).  The argument
// 'buttons' can refer to multiple buttons using the bitwise OR operator |.
uint8_t OrangutanPushbuttons::isPressed(uint8_t buttons)
{
	return (~PINB & buttons) ? 1 : 0;
}
