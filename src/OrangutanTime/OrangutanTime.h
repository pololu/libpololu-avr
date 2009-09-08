/*
  OrangutanTime.h - Library for timing routines, calibrated for Orangutan, which runs at 20 MHz.
  This should *NOT* be used as an Arduino library.
*/

/*
 * Written by Ben Schmidel et al., July 30, 2008.
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

#ifndef OrangutanTime_h
#define OrangutanTime_h

class OrangutanTime
{
  public:

    // Constructor (doesn't do anything).
	OrangutanTime();

	// Resets the ms and us counters to zero.
	static void reset();

	// Returns the number of elapsed milliseconds.
	static unsigned long ms();

	// Returns the number of elapsed microseconds.
	static unsigned long us();

	// Delays for the specified number of milliseconds.
	static void delayMilliseconds(unsigned int milliseconds);

	// Delays for for the specified nubmer of microseconds.
	static inline void delayMicroseconds(unsigned int microseconds)
	{
		__asm__ volatile (
						  "1: push r22"     "\n\t"
						  "   ldi  r22, 4"  "\n\t"
						  "2: dec  r22"     "\n\t"
						  "   brne 2b"      "\n\t"
						  "   pop  r22"     "\n\t"   
						  "   sbiw %0, 1"   "\n\t"
						  "   brne 1b"
						  : "=w" ( microseconds )  
						  : "0" ( microseconds )
						  );  
	}
	
	
  private:

	// Initializes the timer.  This must be called before the
	// milliseconds/microseconds elapsed time functions are used.  It
	// is not required for the delay functions.

	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
  	
	// initializes timer 2 for timing (in a way that is compatible with 
	// other Orangutan libraries)
	static void init2();

};

#ifndef OrangutanTime_cpp
// More convenient aliases for the static class functions.
// These aliases are only accessible when the file is included from
// another C++ file.
inline unsigned long get_ms() { return OrangutanTime::ms(); }
inline unsigned long millis() { return OrangutanTime::ms(); }

inline void delay(unsigned int milliseconds) { OrangutanTime::delayMilliseconds(milliseconds); }
inline void delay_ms(unsigned int milliseconds) { OrangutanTime::delayMilliseconds(milliseconds); }
inline void delay_us(unsigned int microseconds) { OrangutanTime::delayMicroseconds(microseconds); }
inline void delayMicroseconds(unsigned int microseconds) { OrangutanTime::delayMicroseconds(microseconds); }
#endif

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
