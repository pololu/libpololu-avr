/*
  OrangutanDigital.cpp - Library for using the digital I/O lines on the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  The code
	is all inline, which lets it compile to very small, fast, efficient
	assembly code if you use constants as your inputs.  For example,
	the line:
	
		setOutput(3, HIGH);
	
	compiles to the assembly:
	
		sbi 0x0b, 3  ;i.e. PORTD |= 1 << 3;
		sbi 0x0a, 3  ;i.e. DDRD  |= 1 << 3;
	
	In short, if your inputs are constants, you can use this library in
	place of raw digital I/O register manipulation without worrying
	about any significantly increased overhead or processing time.
	Using variables as inputs can increase overhead and processing time,
	but the methods in this library can lead to easier to write (and
	read) programmatic approaches to dealing with digital I/O.
	
	The digital pins on the AVR default to high-impedance inputs after
	a power-up or reset.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
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
 

#include "OrangutanDigital.h"


// constructor
OrangutanDigital::OrangutanDigital()
{

}


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
