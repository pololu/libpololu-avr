/*
  OrangutanLEDs.h - Library for using the LED(s) on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J17/5.d
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
 
 #ifndef OrangutanLEDs_h
#define OrangutanLEDs_h


class OrangutanLEDs
{
  public:

    // constructor (doesn't do anything)
	OrangutanLEDs();

	// turns the Orangutan LED off if 'on' is zero, else this method
	// turns the LED on.  Note that the Baby Orangutan B only has
	// one LED (the red one), so green() will just drive I/O line PD7
	// high or low, depending on the argument.
	static void left(unsigned char on);				// same as red()
	static inline void red(unsigned char on)
	{
		left(on);
	}
	
	static void right(unsigned char on);				// same as green()
	static inline void green(unsigned char on)
	{
		right(on);
	}
};

#endif


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
