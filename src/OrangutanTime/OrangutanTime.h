/*
  OrangutanDelay.cpp - Library for delay, calibrated for Orangutan.  This
  should *NOT* be used as an Arduino library.
*/

/*
 * Written by Ben Schmidel et al., July 30, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18/2
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

#ifdef LIB_POLOLU
// will delay for for the specified nubmer of microseconds if F_CPU is 20 MHz
extern "C" static inline void delayMicroseconds(unsigned int microseconds)
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
#endif // LIB_POLOLU

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
