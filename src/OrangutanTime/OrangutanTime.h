/*
  OrangutanTime.h - Library for timing routines, calibrated for Orangutan, which runs at 20 MHz.
  This should *NOT* be used as an Arduino library.
*/

/*
 * Written by Ben Schmidel et al., July 30, 2008.
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

#ifndef OrangutanTime_h
#define OrangutanTime_h

#ifdef __cplusplus

class OrangutanTime
{
  public:

    // Constructor (doesn't do anything).
	OrangutanTime();

	// Resets the ms and us counters to zero.
	static void reset();
	
	// Returns the number of elapsed ticks (in units of 0.4 us)
	static unsigned long ticks();
	
	// Converts ticks to microseconds
	static unsigned long ticksToMicroseconds(unsigned long numTicks);

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

extern "C" {
#endif

// these are defined in the .cpp file:
unsigned long get_ticks(void);
unsigned long ticks_to_microseconds(unsigned long ticks);
unsigned long get_ms(void);
void delay_ms(unsigned int milliseconds);
void time_reset(void);

// This is inline for efficiency:
static inline void delay_us(unsigned int microseconds)
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

// These are alternative aliases:
static inline void delay(unsigned int milliseconds) { delay_ms(milliseconds); }
static inline unsigned long millis(void) { return get_ms(); }
static inline void delayMicroseconds(unsigned int microseconds) { delay_us(microseconds); }

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
