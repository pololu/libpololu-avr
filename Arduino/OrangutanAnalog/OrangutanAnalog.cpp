/*
  OrangutanAnalog.cpp - Library for using the handling analog inputs on the
	Orangutan LV-168 and Baby Orangutan B.  This library also provides a
	method for reading the temperature sensor on the LV-168.
  Written by Ben Schmidel, May 27, 2008.
  Released into the public domain.
*/

#include <avr/io.h>
#include "OrangutanAnalog.h"


#ifdef LIB_ORANGUTAN

#endif


// constructor
OrangutanAnalog::OrangutanAnalog()
{

}


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


unsigned int OrangutanAnalog::read(unsigned char channel)
{
	startConversion(channel);
	while (isConverting());	// wait for conversion to finish
	return conversionResult();
}


unsigned int OrangutanAnalog::readAvg(unsigned char channel, 
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


unsigned int OrangutanAnalog::readTrimpot()
{
	return readAvg(7, 4);
}


// The temperature sensor reading can be converted into degrees C as follows:
//   T = (Vout - 0.4) / 0.0195 Celcius
// The return value of this function is tenths of a degree Celcius, although
// the accuracy of the temperature sensor is +/- 2 C.
int OrangutanAnalog::readTemperatureC()
{
	unsigned int adcResult = readAvg(6, 4);
	if (getMode())							// if 8-bit mode
		return adcResult * 10 - 200;
	return (adcResult >> 2) * 10 - 200;
}

// The return value of this function is tenths of a degree Farenheit.
int OrangutanAnalog::readTemperatureF()
{
	return (readTemperatureC() * 9 + 3) / 5 + 320;
}


// converts the specified ADC result to millivolts
unsigned int OrangutanAnalog::toMillivolts(unsigned int adcResult)
{
	unsigned long temp = adcResult * 5000UL + 511;
	if (getMode())							// if 8-bit mode
		return temp / 255;
	return temp / 1023;
}
