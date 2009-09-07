/*
  OrangutanAnalog.h - Library for using the analog inputs on the
	Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  This library also
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

#define MODE_8_BIT		1
#define MODE_10_BIT		0
#define TRIMPOT			7
#define TEMP_SENSOR		6

#include "../OrangutanSVP/OrangutanSVP.h"

class OrangutanAnalog
{
  public:

    // constructor (doesn't do anything)
	OrangutanAnalog();

	// set the ADC to run in either 8-bit mode (MODE_8_BIT) or 
	// 10-bit mode (MODE_10_BIT)
	static void setMode(unsigned char mode);
	
	// returns 0 if in 10-bit mode, otherwise returns non-zero.  The return
	// value of this method can be directly compared against the macros
	// MODE_8_BIT and MODE_10_BIT:
	// For example: if (getMode() == MODE_8_BIT) ...
	static unsigned char getMode();

	// take a single analog reading of the specified channel
	static unsigned int read(unsigned char channel);
	
	// take 'sample' readings of the specified channel and return the average
	static unsigned int readAverage(unsigned char channel, 
									  unsigned int samples);
	
	// returns the position of the trimpot (20 readings averaged together).
	// For all devices except the Orangutan SVP, the trimpot is on ADC channel 7.
	// On the Orangutan SVP, the trimpot is on the auxiliary processor, so 
	// calling this function can have side effects related to enabling SPI
	// communication (see the SVP user's guide for more info).
	static unsigned int readTrimpot();

	// the following methods can be used to initiate an ADC conversion
	// that runs in the background, allowing the CPU to perform other tasks
	// while the conversion is in progress.  The procedure is to start a
	// conversion on a channel with startConversion(channel), and then
	// poll isConverting in your main loop.  Once isConverting() returns
	// a zero, the result can be obtained through a call to conversionResult().
	// If use_internal_reference is set to true, the function will use
	// the internal 1.1V voltage reference on the AVR; otherwise it
	// uses the AVCC pin as a reference.
	static void startConversion(unsigned char channel, unsigned char use_internal_reference = 0);

	// returns 1 if the ADC is in the middle of an conversion, otherwise
	// returns 0
	static unsigned char isConverting();
	
	// returns the result of the previous ADC conversion.
	static unsigned int conversionResult();
	
	// converts the specified ADC result to millivolts
	static unsigned int toMillivolts(unsigned int adcResult);
	

#if !defined (__AVR_ATmega324P__) && !defined (__AVR_ATmega1284P__)

	// 3pi: returns the voltage of the battery in millivolts,
	// using 10 averaged samples.
	static unsigned int readBatteryMillivolts_3pi();

	// SV-168/SV-328: returns the voltage of the battery in millivolts,
	// using 10 averaged samples.
	static unsigned int readBatteryMillivolts_SV();

	// This version of the function is included because the 3pi was
	// originally the only supported board with battery voltage
	// sensing.  Instead of using this one, reading the battery
	// voltage should be done with the board-specific functions above.
	static inline unsigned int readBatteryMillivolts()
	{
		return readBatteryMillivolts_3pi();
	}
	
	// returns the output of the Orangutan LV's temperature sensor in tenths of a 
	// degree F or C (20 readings averaged together).  The temperature sensor 
	// is on ADC channel 6.
	static int readTemperatureF();
	static int readTemperatureC();
#endif
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
