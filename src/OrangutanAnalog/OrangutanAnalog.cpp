/*
  OrangutanAnalog.cpp - Library for using the analog inputs on the
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

#include <avr/io.h>
#include "OrangutanAnalog.h"

#include "../OrangutanResources/include/OrangutanModel.h"

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

extern "C" unsigned int analog_read_millivolts(unsigned char channel)
{
	return OrangutanAnalog::readMillivolts(channel);
}

extern "C" unsigned int analog_read_average(unsigned char channel, unsigned int samples)
{
	return OrangutanAnalog::readAverage(channel, samples);
}

extern "C" void start_analog_conversion(unsigned char channel)
{
	OrangutanAnalog::startConversion(channel);
}

extern "C" void start_analog_conversion_using_internal_reference(unsigned char channel)
{
	OrangutanAnalog::startConversion(channel,1);
}

extern "C" unsigned char analog_is_converting()
{
	return OrangutanAnalog::isConverting();
}

extern "C" unsigned int analog_conversion_result()
{
	return OrangutanAnalog::conversionResult();
}

extern "C" unsigned int analog_conversion_result_millivolts()
{
	return OrangutanAnalog::conversionResultMillivolts();
}

extern "C" unsigned int to_millivolts(unsigned int analog_result)
{
	return OrangutanAnalog::toMillivolts(analog_result);
}

extern "C" unsigned int read_trimpot()
{
	return OrangutanAnalog::readTrimpot();
}

#ifdef _ORANGUTAN_SVP
extern "C" unsigned int read_battery_millivolts_svp()
{
	return OrangutanAnalog::readBatteryMillivolts_SVP();
}

#else

extern "C" unsigned int read_battery_millivolts_3pi()
{
	return OrangutanAnalog::readBatteryMillivolts_3pi();
}

extern "C" unsigned int read_battery_millivolts_sv()
{
	return OrangutanAnalog::readBatteryMillivolts_SV();
}

extern "C" unsigned int read_battery_millivolts_sv168()
{
	return OrangutanAnalog::readBatteryMillivolts_SV();
}

extern "C" int read_temperature_f()
{
	return OrangutanAnalog::readTemperatureF();
}

extern "C" int read_temperature_c()
{
	return OrangutanAnalog::readTemperatureC();
}

#endif

#endif

#ifdef _ORANGUTAN_SVP
static unsigned int fromMillivoltsToNormal(unsigned int millivolts);
#endif

#ifdef _ORANGUTAN_SVP
#define ADC_PORT PORTA
#define ADC_DDR  DDRA
#else
#define ADC_PORT PORTC
#define ADC_DDR  DDRC
#endif

#ifdef _ORANGUTAN_SVP
/*  non-zero means the result from the last conversion is stored in millivolts
        in adc_result_millivolts.  The contents of ADCL and ADCH are irrelevant.
	0 means the result from the last conversion is stored in ADCL and ADCH.
	    The contents of adc_result_millivolts are irrelevant.
 */
static unsigned char adc_result_is_in_millivolts = 0;

/*  adc_result_millivolts holds the last ADC result if adc_result_is_in_millivolts
        is non-zero.  David wanted to just store it in ADCL and ADCH, but those
		registers are not writable. */
static unsigned int adc_result_millivolts;
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
	return (ADMUX >> ADLAR) & 1;
}

// returns 1 if the ADC is in the middle of an conversion, otherwise
// returns 0
unsigned char OrangutanAnalog::isConverting()
{
	return (ADCSRA >> ADSC) & 1;
}

// returns the result of the previous ADC conversion.
unsigned int OrangutanAnalog::conversionResult()
{
	#ifdef _ORANGUTAN_SVP
	if (adc_result_is_in_millivolts)
	{
		return fromMillivoltsToNormal(adc_result_millivolts);
	}
	#endif

	if (getMode())				// if left-adjusted (i.e. 8-bit mode)
	{
		return ADCH;			// 8-bit result
	}
	else
	{
		return ADC;				// 10-bit result
	}
}

// returns the result from the previous ADC conversion in millivolts.
unsigned int OrangutanAnalog::conversionResultMillivolts()
{
	#ifdef _ORANGUTAN_SVP
	if (adc_result_is_in_millivolts)
	{
		return adc_result_millivolts;
	}
	#endif

	if (getMode())  // if left-adjusted (i.e. 8-bit mode)
	{
		return toMillivolts(ADCH);
	}
	else
	{
		return toMillivolts(ADC);
	}
}

// the following method can be used to initiate an ADC conversion
// that runs in the background, allowing the CPU to perform other tasks
// while the conversion is in progress.  The procedure is to start a
// conversion on a channel with startConversion(channel), and then
// poll isConverting in your main loop.  Once isConverting() returns
// a zero, the result can be obtained through a call to conversionResult().
// Note: this function will set the pin corresponding to the specified
// channel as an input with the internal pull-up disabled.
void OrangutanAnalog::startConversion(unsigned char channel, unsigned char use_internal_reference)
{
	#ifdef _ORANGUTAN_SVP
	if (channel > 31)
	{
		adc_result_is_in_millivolts = 1;

		if (channel == TRIMPOT){ adc_result_millivolts = OrangutanSVP::getTrimpotMillivolts(); }
		else if (channel == CHANNEL_A){ adc_result_millivolts = OrangutanSVP::getChannelAMillivolts(); }
		else if (channel == CHANNEL_B){ adc_result_millivolts = OrangutanSVP::getChannelBMillivolts(); }
		else if (channel == CHANNEL_C){ adc_result_millivolts = OrangutanSVP::getChannelCMillivolts(); }
		else if (channel == CHANNEL_D){ adc_result_millivolts = OrangutanSVP::getChannelDMillivolts(); }

		return;
	}

	adc_result_is_in_millivolts = 0;

	#else

	// Channel numbers greater than 15 are invalid.
	if (channel > 15)
	{
		return;
	}

	#endif

	ADC_PORT &= ~(1 << channel); // turn off any pull-ups
	ADC_DDR &= ~(1 << channel);  // set I/O line to be an input
	ADCSRA = 0x87;		// bit 7 set: ADC enabled
						// bit 6 clear: don't start conversion
						// bit 5 clear: disable autotrigger
						// bit 4: ADC interrupt flag
						// bit 3 clear: disable ADC interrupt
						// bits 0-2 set: ADC clock prescaler is 128

	ADMUX |= 1 << 6;
	if(use_internal_reference)
	{
		// use the internal voltage reference
		ADMUX |= 1 << 7;
	}
	else
	{
		// use AVCC as a reference
		ADMUX &= ~(1 << 7);
	}

	ADMUX &= 0xE0;		 // set the conversion channel
	ADMUX |= channel;    // assumption: channel is less than 16 (32 on the ATmega324/1284)
	ADCSRA |= 1 << ADSC; // start the conversion
}

static void performConversion(unsigned char channel)
{
	unsigned char tempDDR = ADC_DDR;	// store current I/O state of ADC pin
	unsigned char tempPORT = ADC_PORT;
	OrangutanAnalog::startConversion(channel);
	while (OrangutanAnalog::isConverting());	// wait for conversion to finish
	ADC_PORT = tempPORT;			// restore I/O state of the pin
	ADC_DDR = tempDDR;
}

// take a single analog reading of the specified channel
unsigned int OrangutanAnalog::read(unsigned char channel)
{
	performConversion(channel);
	return conversionResult();
}

// take a single analog reading of the specified channel and return the result in millivolts
unsigned int OrangutanAnalog::readMillivolts(unsigned char channel)
{
	performConversion(channel);
	return conversionResultMillivolts();
}

// take 'samples' readings of the specified channel and return the average
unsigned int OrangutanAnalog::readAverage(unsigned char channel, 
											unsigned int samples)
{
	unsigned int i = samples;
	unsigned long sum = 0;
	unsigned char tempDDR = ADC_DDR;	// store current I/O state of ADC pin
	unsigned char tempPORT = ADC_PORT;

#ifdef _ORANGUTAN_SVP
	if (channel > 31)
	{
		// We have not implemented averaging of the adc readings from the auxiliary
		// processor on the SVP, so we will just return a simple reading.
		return read(channel);
	}
#endif

	startConversion(channel);	// call this first to set the channel
	do
	{
		ADCSRA |= 1 << ADSC;	// start the next conversion on current channel
		while (isConverting());	// wait while converting
		sum += conversionResult();	// sum the results
	} while (--i);
	
	ADC_PORT = tempPORT;			// restore I/O state of the pin
	ADC_DDR = tempDDR;

	if (samples < 64)			// can do the division much faster
		return ((unsigned int)sum + (samples >> 1)) / (unsigned char)samples;
	return (sum + (samples >> 1)) / samples;	// compute the rounded avg
}


// converts the specified ADC result to millivolts
unsigned int OrangutanAnalog::toMillivolts(unsigned int adcResult)
{
	unsigned long temp = adcResult * 5000UL;
	if (getMode())							// if 8-bit mode
		return (temp + 127) / 255;
	return (temp + 511) / 1023;
}

#ifdef _ORANGUTAN_SVP
static unsigned int fromMillivoltsToNormal(unsigned int millivolts)
{
	unsigned long temp;

	if (OrangutanAnalog::getMode())  // if 8-bit mode
	{
		temp = (millivolts * 255UL + 2500) / 5000;
		if (temp > 0xFFu)
		{
			return 0xFFu;
		}
	}
	else
	{
		temp = (millivolts * 1023UL + 2500) / 5000;
		if (temp > 0xFFFFu)
		{
			return 0xFFFFu;
		}
	}
	return temp;
}
#endif

// returns the position of the trimpot (20 readings averaged together).
// For all devices except the Orangutan SVP, the trimpot is on ADC channel 7.
// On the Orangutan SVP, the trimpot is on the auxiliary processor, so 
// calling this function can have side effects related to enabling SPI
// communication (see the SVP user's guide for more info).
unsigned int OrangutanAnalog::readTrimpot()
{
	return readAverage(TRIMPOT, 20);
}

#if !defined(_ORANGUTAN_SVP)

// The temperature sensor reading (on the Orangutan LV) can be converted into degrees C as follows:
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


// Orangutan LV only: The return value of this function is tenths of a degree Celcius.
int OrangutanAnalog::readTemperatureC()
{
	return ((readTemperatureF() - 320) * 5 + 4) / 9;
}


unsigned int OrangutanAnalog::readBatteryMillivolts_3pi()
{
	return readAverage(6,10)*5000L*3/2/1023;
}

unsigned int OrangutanAnalog::readBatteryMillivolts_SV()
{
	return readAverage(6,10)*5000L*3/1023;
}

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
