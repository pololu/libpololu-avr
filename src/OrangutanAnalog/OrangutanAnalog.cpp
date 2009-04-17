/*
  OrangutanAnalog.cpp - Library for using the handling analog inputs on the
	Orangutan LV-168, Baby Orangutan B, or 3pi robot.  This library also 
	provides a method for reading the temperature sensor on the LV-168.
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18/3
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

#include <avr/io.h>
#include "OrangutanAnalog.h"


#ifdef LIB_POLOLU

extern "C" void set_analog_mode(unsigned char mode)
{
	OrangutanAnalog::setMode(mode);
}

extern "C" unsigned char get_analog_mode()
{
	return OrangutanAnalog::getMode();
}

extern "C" unsigned int analog_read(unsigned char channel)
{
	return OrangutanAnalog::read(channel);
}

extern "C" unsigned int analog_read_average(unsigned char channel, unsigned int samples)
{
	return OrangutanAnalog::readAverage(channel, samples);
}

extern "C" unsigned int read_trimpot()
{
	return OrangutanAnalog::readTrimpot();
}

extern "C" int read_temperature_f()
{
	return OrangutanAnalog::readTemperatureF();
}

extern "C" void start_analog_conversion(unsigned char channel)
{
	OrangutanAnalog::startConversion(channel);
}

extern "C" unsigned int read_battery_millivolts_3pi()
{
	return OrangutanAnalog::readBatteryMillivolts_3pi();
}

extern "C" unsigned int read_battery_millivolts_sv168()
{
	return OrangutanAnalog::readBatteryMillivolts_SV168();
}

extern "C" unsigned int read_temperature_c()
{
	return OrangutanAnalog::readTemperatureC();
}

extern "C" unsigned char analog_is_converting()
{
	return OrangutanAnalog::isConverting();
}

extern "C" unsigned int analog_conversion_result()
{
	return OrangutanAnalog::conversionResult();
}

extern "C" unsigned int to_millivolts(unsigned int analog_result)
{
	return OrangutanAnalog::toMillivolts(analog_result);
}

#endif


// constructor
OrangutanAnalog::OrangutanAnalog()
{

}

// set the ADC to run in either 8-bit mode (MODE_8_BIT) or 
// 10-bit mode (MODE_10_BIT)
void OrangutanAnalog::setMode(unsigned char mode)
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
unsigned char OrangutanAnalog::getMode()
{
	return ADMUX & (1 << ADLAR) ? 1 : 0;
}

// returns 1 if the ADC is in the middle of an conversion, otherwise
// returns 0
unsigned char OrangutanAnalog::isConverting()
{
	return ADCSRA & (1 << ADSC);
}

// returns the result of the previous ADC conversion.
inline unsigned int OrangutanAnalog::conversionResult()
{
	if (getMode())				// if left-adjusted (i.e. 8-bit mode)
		return ADCH;			// 8-bit result
	return ADC;				// 10-bit result
}

// the following method can be used to initiate an ADC conversion
// that runs in the background, allowing the CPU to perform other tasks
// while the conversion is in progress.  The procedure is to start a
// conversion on a channel with startConversion(channel), and then
// poll isConverting in your main loop.  Once isConverting() returns
// a zero, the result can be obtained through a call to conversionResult().
// Note: this function will set the pin corresponding to the specified
// channel as an input with the internal pull-up disabled.
void OrangutanAnalog::startConversion(unsigned char channel)
{
	if (channel > 7)
		return;
	PORTC &= ~(1 << channel);	// turn off any pull-ups
	DDRC &= ~(1 << channel);	// set I/O line to be an input
	ADCSRA = 0x87;		// bit 7 set: ADC enabled
						// bit 6 clear: don't start conversion
						// bit 5 clear: disable autotrigger
						// bit 4: ADC interrupt flag
						// bit 3 clear: disable ADC interrupt
						// bits 0-2 set: ADC clock prescaler is 128
	ADMUX &= 0xF0;				// set the conversion channel
	ADMUX |= channel & 0x0F;
	ADCSRA |= 1 << ADSC;		// start the conversion
}


// take a single analog reading of the specified channel
unsigned int OrangutanAnalog::read(unsigned char channel)
{
	unsigned char tempDDRC = DDRC;	// store current I/O state of ADC pin
	unsigned char tempPORTC = PORTC;
	startConversion(channel);
	while (isConverting());	// wait for conversion to finish
	PORTC = tempPORTC;			// restore I/O state of the pin
	DDRC = tempDDRC;
	return conversionResult();
}


// take 'sample' readings of the specified channel and return the average
unsigned int OrangutanAnalog::readAverage(unsigned char channel, 
											unsigned int samples)
{
	unsigned int i = samples;
	unsigned long sum = 0;
	unsigned char tempDDRC = DDRC;	// store current I/O state of ADC pin
	unsigned char tempPORTC = PORTC;

	startConversion(channel);	// call this first to set the channel
	do
	{
		ADCSRA |= 1 << ADSC;	// start the next conversion on current channel
		while (isConverting());	// wait while converting
		sum += conversionResult();	// sum the results
	} while (--i);
	
	PORTC = tempPORTC;			// restore I/O state of the pin
	DDRC = tempDDRC;

	if (samples < 64)			// can do the division much faster
		return ((unsigned int)sum + (samples >> 1)) / (unsigned char)samples;
	return (sum + (samples >> 1)) / samples;	// compute the rounded avg
}


// returns the position of the trimpot (20 readings averaged together).
// The trimpot is on ADC channel 7
unsigned int OrangutanAnalog::readTrimpot()
{
	return readAverage(TRIMPOT, 20);
}


// The temperature sensor reading can be converted into degrees C as follows:
//   T = (Vout - 0.4) / 0.0195 Celcius
// The return value of this function is tenths of a degree Farenheit, although
// the accuracy of the temperature sensor is +/- 2 C.
int OrangutanAnalog::readTemperatureF()
{
	unsigned int adcResult = readAverage(TEMP_SENSOR, 20) * 18;
	if (getMode())							// if 8-bit mode
		return adcResult - 40;
	return ((adcResult + 2) >> 2) - 40;
}


// The return value of this function is tenths of a degree Celcius.
int OrangutanAnalog::readTemperatureC()
{
	return ((readTemperatureF() - 320) * 5 + 4) / 9;
}


// converts the specified ADC result to millivolts
unsigned int OrangutanAnalog::toMillivolts(unsigned int adcResult)
{
	unsigned long temp = adcResult * 5000UL;
	if (getMode())							// if 8-bit mode
		return (temp + 127) / 255;
	return (temp + 511) / 1023;
}

unsigned int OrangutanAnalog::readBatteryMillivolts_3pi()
{
	return readAverage(6,10)*5000L*3/2/1023;
}

unsigned int OrangutanAnalog::readBatteryMillivolts_SV168()
{
	return readAverage(6,10)*5000L*3/1023;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
