/*
  OrangutanAnalog.cpp - Library for using the handling analog inputs on the
	Orangutan LV-168 and Baby Orangutan B.  This library also provides a
	method for reading the temperature sensor on the LV-168.
*/

/*
 * Written by Ben Schmidel, May 27, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J17/5.a
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

extern "C" int read_temperature_c()
{
	return OrangutanAnalog::readTemperatureC();
}

extern "C" void start_analog_conversion(unsigned char channel)
{
	OrangutanAnalog::startConversion(channel);
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


// the following method can be used to initiate an ADC conversion
// that runs in the background, allowing the CPU to perform other tasks
// while the conversion is in progress.  The procedure is to start a
// conversion on a channel with startConversion(channel), and then
// poll isConverting in your main loop.  Once isConverting() returns
// a zero, the result can be obtained through a call to conversionResult().
void OrangutanAnalog::startConversion(unsigned char channel)
{
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
	startConversion(channel);
	while (isConverting());	// wait for conversion to finish
	return conversionResult();
}


// take 'sample' readings of the specified channel and return the average
unsigned int OrangutanAnalog::readAverage(unsigned char channel, 
											unsigned int samples)
{
	unsigned int i = samples;
	unsigned long sum = 0;
	
	startConversion(channel);	// call this first to set the channel
	do
	{
		ADCSRA |= 1 << ADSC;	// start the next conversion on current channel
		while (isConverting());	// wait while converting
		sum += conversionResult();	// sum the results
	} while (--i);
	
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

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
