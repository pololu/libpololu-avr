/*
   OrangutanPulseIn.cpp - Library for reading digital pulses with the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  Pulses
	are measured in ticks, which have units of 0.4 us.  Note that the
	pulse info is updated by pin change interrupts as the pulses occur,
	so unless you use the get_pulse_info() function to freeze a snapshot
	of the current pulse state, values might change on you in unexpected
	ways.  For example, if you want to do something only when there is
	a new high pulse, you might do the following:
	
	if (new_high_pulse(0) && !new_low_pulse(0))
		do something with pulse data
		
	However, a pulse might occur right after you make it past the if
	statement but before you run your conditional code, changing the
	condition to one that would never have made it past the if statement.
*/

/*
 * Written by Ben Schmidel, August 13, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
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
 

#ifndef OrangutanPulseIn_h
#define OrangutanPulseIn_h

#include "../OrangutanTime/OrangutanTime.h"

// possible values for the newPulse member of PulseInputStruct
#define LOW_PULSE			1		// the pulse just completed was a low pulse (pin just went high)
#define HIGH_PULSE			2		// the pulse just completed was a high pulse (pin just went low)
#define ANY_PULSE			3		// newPulse member is not zero


// Structure for storing the port register and approrpiate bitmask for an I/O pin.
// This lets us easily change the output value of the pin represented by the struct.
struct PulseInputStruct
{
	volatile unsigned char* pinRegister;
	unsigned char bitmask;
	volatile unsigned long lastPCTime;
	volatile unsigned char inputState;
	volatile unsigned long lastHighPulse;
	volatile unsigned long lastLowPulse;
	volatile unsigned char newPulse;
};

#ifdef __cplusplus

class OrangutanPulseIn
{
  public:

    // constructor (doesn't do anything)
	OrangutanPulseIn();
	
	// destructor (frees allocated memory)
	~OrangutanPulseIn();
		
	static unsigned char start(const unsigned char *pulsePins, unsigned char numPins);
		
	static void getPulseInfo(unsigned char idx, struct PulseInputStruct* pulseInfo);

	static inline unsigned char newHighPulse(unsigned char idx)
	{
		return newPulse(idx, HIGH_PULSE);
	}

	static inline unsigned char newLowPulse(unsigned char idx)
	{
		return newPulse(idx, LOW_PULSE);
	}

	static inline unsigned char newPulse(unsigned char idx)
	{
		return newPulse(idx, ANY_PULSE);
	}
	
	static unsigned long getLastHighPulse(unsigned char idx);
	
	static unsigned long getLastLowPulse(unsigned char idx);
	
	static void getCurrentState(unsigned char idx, unsigned long* pulseWidth, unsigned char* state);

	static inline unsigned long toMicroseconds(unsigned long pulse)
	{
		return OrangutanTime::ticksToMicroseconds(pulse);
	}
	
	static void stop();
	
	
  private:

	static unsigned char newPulse(unsigned char idx, unsigned char state);
};

extern "C" {
#endif // __cplusplus

// use of pulse_in_init() is discouraged; use pulse_in_start() instead
unsigned char pulse_in_start(const unsigned char *pulsePins, unsigned char numPins);
void get_pulse_info(unsigned char idx, struct PulseInputStruct* pulse_info);
unsigned char new_pulse(unsigned char idx);
unsigned char new_high_pulse(unsigned char idx);
unsigned char new_low_pulse(unsigned char idx);
unsigned long get_last_high_pulse(unsigned char idx);
unsigned long get_last_low_pulse(unsigned char idx);
void get_current_pulse_state(unsigned char idx, unsigned long* pulse_width, unsigned char* state);
unsigned long pulse_to_microseconds(unsigned long pulse);
void pulse_in_stop(void);

#ifdef __cplusplus
}
#endif

#endif // OrangutanPulseIn_h

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
