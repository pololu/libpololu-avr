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

#define BUTTON_C	(1 << PB5)		// Arduino pin 13
#define BUTTON_B	(1 << PB4)		// Arduino pin 12
#define BUTTON_A	(1 << PB1)		// Arduino pin 9

#define ALL_BUTTONS		(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON)


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

	// returns 1 (true) if any of the specified buttons is currently being
	// pressed.  Otherwise this method returns 0 (false).  The argument
	// 'buttons' can refer to multiple buttons using the bitwise OR operator |.
	static unsigned char isPressed(unsigned char buttons);

	
  private:

	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
  	
	// initializes I/O pins for use as button inputs
	static void init2();
};

#endif

