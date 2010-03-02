/*
  leds.h - Library for using the LED(s) on the
      Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
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

#ifndef OrangutanLEDs_h
#define OrangutanLEDs_h

#include "digital.h"
#include "OrangutanResources/include/OrangutanModel.h"

#ifdef _ORANGUTAN_X2
#define RED_LED		IO_C7
#define GREEN_LED		IO_C5
#define RED_LED2		IO_C3
#define GREEN_LED2	IO_C2
#define YELLOW_LED	IO_C0

#elif defined(_ORANGUTAN_SVP)
#define RED_LED		IO_D1
#define GREEN_LED		IO_C4

#else
#define RED_LED		IO_D1
#define GREEN_LED		IO_D7
#endif


// turns the target LED off if 'on' is zero, else this method
// turns the LED on.  Note that the Baby Orangutan B only has
// one LED (the red one), so green() will just drive I/O line PD7
// high or low, depending on the argument.
static inline void red_led(unsigned char on)
{
	#if defined (_ORANGUTAN_SVP)	// red LED turns on when driven low
	if (on == TOGGLE)
		set_digital_output(RED_LED, TOGGLE);
	else if (on == LOW)
		set_digital_output(RED_LED, HIGH);
	else
		set_digital_output(RED_LED, LOW);
	#else					// else red LED turns on when driven high
	set_digital_output(RED_LED, on);
	#endif
}
	
static inline void green_led(unsigned char on)
{
	#if defined (_ORANGUTAN_SVP)	// green LED turns on when driven low
	if (on == TOGGLE)
		set_digital_output(GREEN_LED, TOGGLE);
	else if (on == LOW)
		set_digital_output(GREEN_LED, HIGH);
	else
		set_digital_output(GREEN_LED, LOW);
	#else					// else green LED turns on when driven high
	set_digital_output(GREEN_LED, on);
	#endif
}

static inline void left_led(unsigned char on)
{
	#ifdef _ORANGUTAN_SVP
	green_led(on);
	#else
	red_led(on);
	#endif
}

static inline void right_led(unsigned char on)
{
	#ifdef _ORANGUTAN_SVP
	red_led(on);
	#else
	green_led(on);
	#endif
}

#ifdef _ORANGUTAN_X2
// functions for controlling the additional LED red, green, and yellow
// LEDs on the Orangutan X2
static inline void red_led2(unsigned char on)
{
	set_digital_output(RED_LED2, on);
}

static inline void green_led2(unsigned char on)
{
	set_digital_output(GREEN_LED2, on);
}

static inline void yellow_led(unsigned char on)
{
	set_digital_output(YELLOW_LED, on);
}
#endif //_ORANGUTAN_X2


#endif