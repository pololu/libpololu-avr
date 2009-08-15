/*
  OrangutanPulseIn.h - Library for reading digital pulses with the
	Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  This library
	can be used to read RC inputs.  To use this library, you must
	repeatedly call the update() function in your main loop more often
	than your configured maximum pulse length.  If this is done,
	pulses longer than the maximum pulse length will be reported as having
	a duration of 0xFFFF (65535).  If update() is not called often enough,
	pulses longer than the maximum pulse length can be reported as having
	any pulse length from 0 and 65535, depending on how the timer has
	overflowed.
*/

/*
 * Written by Ben Schmidel, August 13, 2009.
 * Copyright (c) 2009 Pololu Corporation. For more information, see
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


// possible values for the newPulse member of PulseInputStruct
#define LOW_PULSE			1		// the pulse just completed was a low pulse (pin just went high)
#define HIGH_PULSE		2		// the pulse just completed was a high pulse (pin just went low)

// possible values for the argument to the setMaxPulseLength method
#define MAX_PULSE_3MS		1	// can measure pulses with 0.05 us resolution
#define MAX_PULSE_26MS		2	// can measure pulses with 0.4 us resolution
#define MAX_PULSE_210MS		3	// can measure pulses with 3.2 us resolution
#define MAX_PULSE_839MS		4	// can measure pulses with 12.8 us resolution
#define MAX_PULSE_3355MS	5	// can measure pulses with 51.2 us resolution


// Structure for storing the port register and approrpiate bitmask for an I/O pin.
// This lets us easily change the output value of the pin represented by the struct.
struct PulseInputStruct
{
	volatile unsigned char* pinRegister;
	unsigned char bitmask;
	volatile unsigned int lastPCTime;
	volatile unsigned char inputState;
	volatile unsigned int curPulseWidth;
	volatile unsigned int lastHighPulse;
	volatile unsigned int lastLowPulse;
	volatile unsigned char newPulse;
};



class OrangutanPulseIn
{
  public:

    // constructor (doesn't do anything)
	OrangutanPulseIn();
	
	// destructor (frees allocated memory)
	~OrangutanPulseIn();
	
	static unsigned char init(const unsigned char *pulsePins, unsigned char numPins, unsigned char maxLengthEnum);
	
	static void update();
	
	static void setMaxPulseLength(unsigned char maxLengthEnum);
	
	static struct PulseInputStruct getPulseInfo(unsigned char idx);

};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
