/*
  OrangutanPushbuttons.cpp - Library for using the three user pushbuttons on the
      Orangutan LV, SV, SVP, X2, or 3pi robot.
*/

/*
 * Written by Ben Schmidel, May 23, 2008.
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
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
#include "../OrangutanResources/include/OrangutanModel.h"


// constructor

OrangutanPushbuttons::OrangutanPushbuttons()
{

}


#ifndef ARDUINO
#include "../OrangutanTime/OrangutanTime.h"	// provides access to delay routines
#else
#include <Arduino.h> // provides access to delay() and delayMicroseconds()
#endif


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

extern "C" unsigned char get_single_debounced_button_press(unsigned char buttons)
{
	return OrangutanPushbuttons::getSingleDebouncedPress(buttons);
}

extern "C" unsigned char get_single_debounced_button_release(unsigned char buttons)
{
	return OrangutanPushbuttons::getSingleDebouncedRelease(buttons);
}


inline void OrangutanPushbuttons::init()
{
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
}

// initializes I/O pins for use as button inputs
void OrangutanPushbuttons::init2()
{
	BUTTON_DDR &= ~ALL_BUTTONS;		// set the pushbutton pins to be inputs
#ifndef _ORANGUTAN_X2
	BUTTON_PORT |= ALL_BUTTONS;		// enable pullups on the pushbutton pins
#endif
	delayMicroseconds(5);			// give pullups time to stabilize
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
		while (!(BUTTONS_DOWN & buttons))	// wait for a button to be pressed
			;
		delay(10);						// debounce the button press
	}
	while (!(BUTTONS_DOWN & buttons));		// if button isn't still pressed, loop
	return BUTTONS_DOWN & buttons;			// return the pressed button(s)
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
		while (!(BUTTONS_UP & buttons))	// wait for a button to be released
			;
			delay(10);						// debounce the button release
	}
	while (!(BUTTONS_UP & buttons));		// if button isn't still released, loop
	return BUTTONS_UP & buttons;			// return the released button(s)
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
	return BUTTONS_DOWN & buttons;
}


// Uses a finite state machine to detect a single button press and return
// the pressed button.  It requires the button to be up for at least 15 ms
// and then down for at least 15 ms before reporting the press.  This function
// handles all necessary debouncing and should be called repeatedly in a loop
// with the same button-mask argument.  The argument 'buttons' can refer to 
// multiple buttons using the bitwise OR operator | or (e.g.
// TOP_BUTTON | MIDDLE_BUTTON) or the value ANY_BUTTON.
unsigned char OrangutanPushbuttons::getSingleDebouncedPress(unsigned char buttons)
{
	static unsigned char state = 0;
	static unsigned long prevTimeMillis = 0;
	static unsigned char mask = 0;
	
	unsigned char buttonsDown = BUTTONS_DOWN;
	unsigned long timeMillis = millis();
	
	init();
	switch (state)
	{
		case 0:
			if (~buttonsDown & buttons)				// if one of the specified buttons is up
			{
				mask = ~buttonsDown & buttons;		// mask becomes all of masked up buttons
				prevTimeMillis = timeMillis;
				state = 1;
			}
			break;
			
		case 1:
			if (timeMillis - prevTimeMillis >= 15)	// if 15 ms or longer has elapsed
			{
				if (~buttonsDown & mask)			// and if a masked button is still up
				{
					state = 2;						// proceed to next state
					mask = ~buttonsDown & mask;		// new mask becomes all of masked up buttons
				}
				else
				{
					state = 0;						// go back to previous (initial) state
				}
			}
			break;
			
		case 2:
			if (buttonsDown & mask)					// if a masked button is now down
			{
				state = 3;							// proceed to next state
				mask = buttonsDown & mask;			// new mask becomes all of masked down buttons
				prevTimeMillis = timeMillis;
			}
			else if (mask != (~buttonsDown & buttons))	// if our mask becomes inaccurate
			{
				state = 0;							// go back to the initial state
			}
			break;
			
		case 3:
			if (timeMillis - prevTimeMillis >= 15)	// if 15 ms or longer has elapsed
			{
				if (buttonsDown & mask)				// and if a masked button is still down
				{
					state = 0;						// next state becomes initial state
					return buttonsDown & mask;		// return masked down buttons
				}
				else
				{
					state = 2;						// go back to previous state
				}
			}
			break;
	}
	
	return 0;
}


// Uses a finite state machine to detect a single button release and return
// the released button.  It requires the button to be down for at least 15 ms
// and then up for at least 15 ms before reporting the press.  This function
// handles all necessary debouncing and should be called repeatedly in a loop
// with the same button-mask argument.  The argument 'buttons' can refer to 
// multiple buttons using the bitwise OR operator | or (e.g.
// TOP_BUTTON | MIDDLE_BUTTON) or the value ANY_BUTTON.
unsigned char OrangutanPushbuttons::getSingleDebouncedRelease(unsigned char buttons)
{
	static unsigned char state = 0;
	static unsigned long prevTimeMillis = 0;
	static unsigned char mask = 0;
	
	unsigned char buttonsUp = BUTTONS_UP;
	unsigned long timeMillis = millis();
	
	init();
	switch (state)
	{
		case 0:
			if (~buttonsUp & buttons)				// if one of the specified buttons is down
			{
				mask = ~buttonsUp & buttons;		// mask becomes all of masked down buttons
				prevTimeMillis = timeMillis;
				state = 1;
			}
			break;
			
		case 1:
			if (timeMillis - prevTimeMillis >= 15)	// if 15 ms or longer has elapsed
			{
				if (~buttonsUp & mask)				// and if a masked button is still down
				{
					state = 2;						// proceed to next state
					mask = ~buttonsUp & mask;		// new mask becomes all of masked down buttons
				}
				else
				{
					state = 0;						// go back to previous (initial) state
				}
			}
			break;
			
		case 2:
			if (buttonsUp & mask)					// if a masked button is now up
			{
				state = 3;							// proceed to next state
				mask = buttonsUp & mask;			// new mask becomes all of masked up buttons
				prevTimeMillis = timeMillis;
			}
			else if (mask != (~buttonsUp & buttons))	// if our mask becomes inaccurate
			{
				state = 0;							// go back to the initial state
			}
			break;

			break;
			
		case 3:
			if (timeMillis - prevTimeMillis >= 15)	// if 15 ms or longer has elapsed
			{
				if (buttonsUp & mask)				// and if a masked button is still up
				{
					state = 0;						// next state becomes initial state
					return buttonsUp & mask;		// return masked up buttons
				}
				else
				{
					state = 2;						// go back to previous state
				}
			}
			break;
	}
	
	return 0;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
