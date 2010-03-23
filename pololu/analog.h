/*
  analog.h - Library for using the analog inputs on the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  This library also
	provides a method for reading the temperature sensor on the LV-168.
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

#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#include <avr/io.h>
#include "OrangutanResources/include/OrangutanModel.h"

#define MODE_8_BIT		1
#define MODE_10_BIT		0

// ADC Channels

#ifdef _ORANGUTAN_SVP

#define TRIMPOT   128
#define CHANNEL_A 129
#define CHANNEL_B 130
#define CHANNEL_C 131
#define CHANNEL_D 132

#else

#define TRIMPOT			7
#define TEMP_SENSOR		6

#endif

static inline void set_analog_mode(unsigned char mode)
{
	if (mode == MODE_10_BIT)
		ADMUX &= ~(1 << ADLAR);	// right-adjust result (ADC has result)
	else
		ADMUX |= 1 << ADLAR;		// left-adjust result (ADCH has result)	
}
static inline unsigned char get_analog_mode()
{
	return (ADMUX >> ADLAR) & 1;
}
unsigned int analog_read(unsigned char channel);
unsigned int analog_read_millivolts(unsigned char channel);
unsigned int analog_read_average(unsigned char channel, unsigned int samples);
unsigned int analog_read_average_millivolts(unsigned char channel, unsigned int samples);
void start_analog_conversion(unsigned char channel);
static inline unsigned char analog_is_converting()
{
	return (ADCSRA >> ADSC) & 1;
}
unsigned int analog_conversion_result();
unsigned int analog_conversion_result_millivolts();
void set_millivolt_calibration(unsigned int calibration);
unsigned int read_vcc_millivolts();
unsigned int to_millivolts(unsigned int analog_result);
unsigned int read_trimpot();
unsigned int read_trimpot_millivolts();

#ifdef _ORANGUTAN_SVP

unsigned int read_battery_millivolts_svp();
static inline unsigned int read_battery_millivolts()
{
  return read_battery_millivolts_svp();
}

#elif defined(_ORANGUTAN_X2)

unsigned int read_battery_millivolts_x2();
static inline unsigned int read_battery_millivolts()
{
  return read_battery_millivolts_x2();
}

#else

int read_temperature_f();
int read_temperature_c();

unsigned int read_battery_millivolts_3pi();
static inline unsigned int read_battery_millivolts()
{
  return read_battery_millivolts_3pi();
}
unsigned int read_battery_millivolts_sv();
unsigned int read_battery_millivolts_sv168();

#endif

#endif
