/*
  OrangutanAnalog.h - Library for using the analog inputs on the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  This library
	also provides a method for reading the temperature sensor on the LV-168.
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
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

#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#include <avr/io.h>
#include "../OrangutanResources/include/OrangutanModel.h"

#ifdef __cplusplus
#ifdef _ORANGUTAN_SVP
#include "../OrangutanSVP/OrangutanSVP.h"
#endif
#endif

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

#ifdef __cplusplus

class OrangutanAnalog
{
  public:

    // constructor (doesn't do anything)
	OrangutanAnalog();

	// set the ADC to run in either 8-bit mode (MODE_8_BIT) or 
	// 10-bit mode (MODE_10_BIT)
	static inline void setMode(unsigned char mode)
	{
		if (mode == MODE_10_BIT)
			ADMUX &= ~(1 << ADLAR);	// right-adjust result (ADC has result)
		else
			ADMUX |= 1 << ADLAR;		// left-adjust result (ADCH has result)	
	}
	
	// returns 0 if in 10-bit mode, otherwise returns non-zero.  The return
	// value of this method can be directly compared against the macros
	// MODE_8_BIT and MODE_10_BIT:
	// For example: if (getMode() == MODE_8_BIT) ...
	static inline unsigned char getMode()
	{
		return (ADMUX >> ADLAR) & 1;
	}

	// take a single analog reading of the specified channel
	static unsigned int read(unsigned char channel);

	// take a single analog reading of the specified channel and return result in millivolts
	static unsigned int readMillivolts(unsigned char channel);

	// take 'sample' readings of the specified channel and return the average
	static unsigned int readAverage(unsigned char channel, 
									  unsigned int samples);
									  
	static inline unsigned int readAverageMillivolts(unsigned char channel, unsigned int samples)
	{
	#ifdef _ORANGUTAN_SVP
		if (channel > 31)
		{
			// We have not implemented averaging of the adc readings from the auxiliary
			// processor on the SVP, so we will just return a simple reading.
			return readMillivolts(channel);
		}
	#endif
		return toMillivolts(readAverage(channel, samples));
	}
	
	// returns the position of the trimpot (20 readings averaged together).
	// For all devices except the Orangutan SVP, the trimpot is on ADC channel 7.
	// On the Orangutan SVP, the trimpot is on the auxiliary processor, so 
	// calling this function can have side effects related to enabling SPI
	// communication (see the SVP user's guide for more info).
	static inline unsigned int readTrimpot()
	{
		return readAverage(TRIMPOT, 20);
	}

	static inline unsigned int readTrimpotMillivolts()
	{
	#ifdef _ORANGUTAN_SVP
		return OrangutanSVP::getTrimpotMillivolts();
	#else
		return toMillivolts(readTrimpot());
	#endif
	}

	// the following method can be used to initiate an ADC conversion
	// that runs in the background, allowing the CPU to perform other tasks
	// while the conversion is in progress.  The procedure is to start a
	// conversion on a channel with startConversion(channel), and then
	// poll isConverting in your main loop.  Once isConverting() returns
	// a zero, the result can be obtained through a call to conversionResult().
	// If use_internal_reference is set to true, the function will use the
	// internal 1.1V voltage reference on the ATmega48/168/328 or the internal
	// 2.56V voltage reference on the ATmega324/644/1284; otherwise, it uses
	// the AVCC pin as a reference.
	// *** NOTE ***: Some Orangutans and 3pis have their AREF pin connected directly to VCC.
	//  On these Orangutans, you must not use the internal voltage reference as
	//  doing so will short the internal reference voltage to VCC and could damage
	//  the AVR.  It is safe to use the internal reference voltage on the
	//  Orangutan SVP.
	static void startConversion(unsigned char channel, unsigned char use_internal_reference = 0);

	// returns 1 if the ADC is in the middle of an conversion, otherwise
	// returns 0
	static inline unsigned char isConverting()
	{
		return (ADCSRA >> ADSC) & 1;
	}
	
	// returns the result of the previous ADC conversion.
	static unsigned int conversionResult();

	// returns the result of the previous ADC conversion in millivolts.
	static unsigned int conversionResultMillivolts();
	
	// sets the value used to calibrate the conversion from ADC reading
	// to millivolts.  The argument calibration should equal VCC in millivolts,
	// which can be automatically measured using the function readVCCMillivolts():
	// e.g. setMillivoltCalibration(readVCCMillivolts());
	static void setMillivoltCalibration(unsigned int calibration);

	// averages ten ADC readings of the fixed internal 1.1V bandgap voltage
	// and computes VCC from the results.  This function returns VCC in millivolts.
	// Channel 14 is internal 1.1V BG on ATmega48/168/328, but bit 5 of ADMUX is
	// not used, so channel 30 is equivalent to channel 14.  Channel 30 is the internal
	// 1.1V BG on ATmega324/644/1284.
	static unsigned int readVCCMillivolts();

	// converts the specified ADC result to millivolts
	static unsigned int toMillivolts(unsigned int adcResult);

	// SVP: returns the voltage of the battery in millivolts, as retrieved from
	// the auxiliary processor.  Calling this function will have side effects
	// related to enabling the SPI module.  See the SVP User's Guide for details.
#ifdef _ORANGUTAN_SVP
	static inline unsigned int readBatteryMillivolts_SVP()
	{
		return OrangutanSVP::getBatteryMillivolts();
	}
	
	static inline unsigned int readBatteryMillivolts()
	{
		return readBatteryMillivolts_SVP();
	}
	
#elif defined(_ORANGUTAN_X2)

	// X2: returns the voltage of the battery in millivolts using
	// 10 averaged samples.
	static unsigned int readBatteryMillivolts_X2();
	
	static inline unsigned int readBatteryMillivolts()
	{
		return readBatteryMillivolts_X2();
	}
	
#else

	// 3pi: returns the voltage of the battery in millivolts using
	// 10 averaged samples.
	static unsigned int readBatteryMillivolts_3pi();

	// SV-168/SV-328: returns the voltage of the battery in millivolts
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
	
#endif // _ORANGUTAN_SVP
};

extern "C" {
#endif // __cplusplus

static inline void set_analog_mode(unsigned char mode)
{
	if (mode == MODE_10_BIT)
		ADMUX &= ~(1 << ADLAR);	// right-adjust result (ADC has result)
	else
		ADMUX |= 1 << ADLAR;		// left-adjust result (ADCH has result)	
}
static inline unsigned char get_analog_mode(void)
{
	return (ADMUX >> ADLAR) & 1;
}
unsigned int analog_read(unsigned char channel);
unsigned int analog_read_millivolts(unsigned char channel);
unsigned int analog_read_average(unsigned char channel, unsigned int samples);
unsigned int analog_read_average_millivolts(unsigned char channel, unsigned int samples);
void start_analog_conversion(unsigned char channel);
static inline unsigned char analog_is_converting(void)
{
	return (ADCSRA >> ADSC) & 1;
}
unsigned int analog_conversion_result(void);
unsigned int analog_conversion_result_millivolts(void);
void set_millivolt_calibration(unsigned int calibration);
unsigned int read_vcc_millivolts(void);
unsigned int to_millivolts(unsigned int analog_result);
unsigned int read_trimpot(void);
unsigned int read_trimpot_millivolts(void);

#ifdef _ORANGUTAN_SVP

unsigned int read_battery_millivolts_svp(void);
static inline unsigned int read_battery_millivolts(void)
{
  return read_battery_millivolts_svp();
}

#elif defined(_ORANGUTAN_X2)

unsigned int read_battery_millivolts_x2(void);
static inline unsigned int read_battery_millivolts(void)
{
  return read_battery_millivolts_x2();
}

#else

int read_temperature_f(void);
int read_temperature_c(void);

unsigned int read_battery_millivolts_3pi(void);
static inline unsigned int read_battery_millivolts(void)
{
  return read_battery_millivolts_3pi();
}
unsigned int read_battery_millivolts_sv(void);
unsigned int read_battery_millivolts_sv168(void);

#endif

#ifdef __cplusplus
}
#endif

#endif // OrangutanAnalog_h

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
