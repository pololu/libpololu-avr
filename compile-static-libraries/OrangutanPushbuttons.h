/*
  OrangutanPushbuttons.h - Library for using the thee user pushbuttons on the
      Orangutan LV-168
  Written by Ben Schmidel, May 23, 2008.
  Released into the public domain.
*/
#ifndef OrangutanPushbuttons_h
#define OrangutanPushbuttons_h


#define TOP_BUTTON		(1 << PB5)		// Arduino pin 13
#define MIDDLE_BUTTON	(1 << PB4)		// Arduino pin 12
#define BOTTOM_BUTTON	(1 << PB1)		// Arduino pin 9
#define ALL_BUTTONS		(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON)


class OrangutanPushbuttons
{
  public:

    // constructor
	OrangutanPushbuttons();

	// initializes I/O pins for use as button inputs
	void init();

	// wait for any of the specified buttons to be pressed, at which point
	// execution will return from this method.  The argument 'buttons' can
	// refer to multiple buttons using the bitwise OR operator |.  The
	// return value is the ID of the button that was pressed.  Note that
	// this method takes care of button debouncing.
	// Example: waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
	uint8_t waitForPress(uint8_t buttons);

	// wait for any of the specified buttons to be released, at which point
	// execution will return from this method.
	uint8_t waitForRelease(uint8_t buttons);

	// returns 1 (true) if any of the specified buttons is currently being
	// pressed.  Otherwise this method returns 0 (false).  The argument
	// 'buttons' can refer to multiple buttons using the bitwise OR operator |.
	uint8_t isPressed(uint8_t buttons);

	// Waits for the specified buttons to be pressed and releaed.
	uint8_t waitForButton(uint8_t buttons);
};

#endif

