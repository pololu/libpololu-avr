/*
  OrangutanPushbuttons.h - Library for using the three user pushbuttons on the
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

#ifndef OrangutanPushbuttons_h
#define OrangutanPushbuttons_h

#include "../OrangutanResources/include/OrangutanModel.h"

#ifdef _ORANGUTAN_SVP

// Orangutan SVP definitions (pushing button makes input low)
#define TOP_BUTTON		(1 << PORTC5)
#define MIDDLE_BUTTON	(1 << PORTC3)
#define BOTTOM_BUTTON	(1 << PORTC2)

#define BUTTON_DDR		DDRC
#define BUTTON_PORT		PORTC
#define BUTTON_PIN		PINC

#define BUTTONS_DOWN		(~BUTTON_PIN)
#define BUTTONS_UP		BUTTON_PIN

#elif defined(_ORANGUTAN_X2)

// Orangutan X2 definitions (pushing button makes input high)
#define TOP_BUTTON		(1 << PORTC6)
#define MIDDLE_BUTTON	(1 << PORTC4)
#define BOTTOM_BUTTON	(1 << PORTC1)

#define BUTTON_DDR		DDRC
#define BUTTON_PORT		PORTC
#define BUTTON_PIN		PINC

#define BUTTONS_DOWN		BUTTON_PIN
#define BUTTONS_UP		(~BUTTON_PIN)

#else


// Orangutan LV, SV, and 3pi definitions (pushing button makes input low)
#define TOP_BUTTON		(1 << PORTB5)
#define MIDDLE_BUTTON	(1 << PORTB4)
#define BOTTOM_BUTTON	(1 << PORTB1)

#define BUTTON_PORT		PORTB
#define BUTTON_DDR		DDRB
#define BUTTON_PIN		PINB

#define BUTTONS_DOWN		(~BUTTON_PIN)
#define BUTTONS_UP		BUTTON_PIN

#endif


#define BUTTON_C		TOP_BUTTON
#define BUTTON_B		MIDDLE_BUTTON
#define BUTTON_A		BOTTOM_BUTTON

#define ALL_BUTTONS		(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON)
#define ANY_BUTTON		ALL_BUTTONS

#ifdef __cplusplus

class OrangutanPushbuttons
{
  public:

    // constructor (doesn't do anything)
	OrangutanPushbuttons();

	// wait for any of the specified buttons to be pressed, at which point
	// execution will return from this method.  The argument 'buttons' can
	// refer to multiple buttons using the bitwise OR operator |.  The
	// return value is the ID of the button that was pressed.  Note that
	// this method takes care of button debouncing.
	// Example: waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
	static unsigned char waitForPress(unsigned char buttons);

	// wait for any of the specified buttons to be released, at which point
	// execution will return from this method.
	static unsigned char waitForRelease(unsigned char buttons);

	// Waits for the any of the specified buttons to be pressed and released.
	static unsigned char waitForButton(unsigned char buttons);

	// returns the value of any specified buttons that are currently pressed,
	// or 0.  For example, if the top and middle buttons are both pressed, this
	// method will return (TOP_BUTTON | MIDDLE_BUTTON).  The argument 'buttons'
	// can refer to multiple buttons using the bitwise OR operator |.
	static unsigned char isPressed(unsigned char buttons);
	
	// These are more complex functions that make it easy to perform
	// button-triggered actions from within a loop, since they only return
	// a non-zero value once for each button transition from up to down or
	// down to up.
	static unsigned char getSingleDebouncedPress(unsigned char buttons);
	static unsigned char getSingleDebouncedRelease(unsigned char buttons);
	
  private:

	static void init();
  	
	// initializes I/O pins for use as button inputs
	static void init2();
};

extern "C" {
#endif	// __cplusplus

void buttons_init(void);
unsigned char wait_for_button_press(unsigned char buttons);
unsigned char wait_for_button_release(unsigned char buttons);
unsigned char wait_for_button(unsigned char buttons);
unsigned char button_is_pressed(unsigned char buttons);
unsigned char get_single_debounced_button_press(unsigned char buttons);
unsigned char get_single_debounced_button_release(unsigned char buttons);

#ifdef __cplusplus
}
#endif

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
