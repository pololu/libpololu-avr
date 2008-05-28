/*
  OrangutanLEDs.h - Library for using the LED(s) on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
  Written by Ben Schmidel, May 27, 2008.
  Released into the public domain.
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

