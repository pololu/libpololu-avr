/*
  PololuWheelEncoders.h - Library for using Pololu Wheel Encoders.
*/
	
/*
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

#ifndef PololuWheelEncoders_h
#define PololuWheelEncoders_h

#ifdef __cplusplus

class PololuWheelEncoders
{
  public:
	/*
	 * Constructor: does nothing.
	 */
	PololuWheelEncoders() { }

	/*
	 * Initializes the wheel encoders.  The four arguments are the
	 * four pins that the two wheel encoders are connected to, according
	 * to the Arduino numbering: Arduino digital pins 0 - 7 correpsond
	 * to port D pins PD0 - PD7, respectively.  Arduino digital pins 8
	 * - 13 correspond to port B pins PB0 - PB5.  Arduino analog
	 * inputs 0 - 5 are referred to as digital pins 14 - 19 (these are
	 * the enumerations you should use for this library) and
	 * correspond to port C pins PC0 - PC5.
	 *
	 * The arguments are named m1a, m2a, etc. with the intention
	 * that when motor M1 is spinning forward, pin m1a will
	 * change before pin m1b.  However, it is difficult to get them
	 * all correct on the first try, and you might have to
	 * experiment.
	 * 
	 * init() may be called multiple times.
	 */
	static void init(unsigned char m1a, unsigned char m1b, unsigned char m2a, unsigned char m2b);

	/*
	 * Encoder counts are returned as integers.  For the Pololu wheel
	 * encoders, the resolution is about 3mm/count, so this allows a
	 * maximum distance of 32767*3mm or about 100m.  For longer
	 * distances, you will need to occasionally reset the counts using
	 * the functions below.
	 */
	static int getCountsM1();
	static int getCountsM2();

	/*
	 * These functions get the number of counts and reset the stored
	 * number to zero.
	 */
	static int getCountsAndResetM1();
	static int getCountsAndResetM2();

	/*
	 * These functions check whether there has been an error on M1 or
	 * M2; that is, if both m1a/m1b or m2a/m2b changed simultaneously.
	 * They return 1 if there was an error, then reset the error
	 * flag.
	 */
	static unsigned char checkErrorM1();
	static unsigned char checkErrorM2();
};

extern "C" {
#endif // __cplusplus

void encoders_init(unsigned char m1a, unsigned char m1b, unsigned char m2a, unsigned char m2b);
int encoders_get_counts_m1(void);
int encoders_get_counts_m2(void);
int encoders_get_counts_and_reset_m1(void);
int encoders_get_counts_and_reset_m2(void);
int encoders_check_error_m1(void);
int encoders_check_error_m2(void);

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
