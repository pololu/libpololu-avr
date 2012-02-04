/*
  OrangutanAnalog.cpp - Library for using the analog inputs on the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  This library also 
	provides a method for reading the temperature sensor on the LV-168.
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

#include <avr/io.h>
#include "OrangutanAnalog.h"

#include "../OrangutanResources/include/OrangutanModel.h"


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

extern "C" unsigned int analog_read_average_millivolts(unsigned char channel, unsigned int samples)
{
	return OrangutanAnalog::readAverageMillivolts(channel, samples);
}

extern "C" void start_analog_conversion(unsigned char channel)
{
	OrangutanAnalog::startConversion(channel);
}

extern "C" unsigned int analog_conversion_result()
{
	return OrangutanAnalog::conversionResult();
}

extern "C" unsigned int analog_conversion_result_millivolts()
{
	return OrangutanAnalog::conversionResultMillivolts();
}

extern "C" void set_millivolt_calibration(unsigned int calibration)
{
	OrangutanAnalog::setMillivoltCalibration(calibration);
}

extern "C" unsigned int read_vcc_millivolts()
{
	return OrangutanAnalog::readVCCMillivolts();
}

extern "C" unsigned int to_millivolts(unsigned int analog_result)
{
	return OrangutanAnalog::toMillivolts(analog_result);
}

extern "C" unsigned int read_trimpot()
{
	return OrangutanAnalog::readTrimpot();
}

extern "C" unsigned int read_trimpot_millivolts()
{
	return OrangutanAnalog::readTrimpotMillivolts();
}

#ifdef _ORANGUTAN_SVP

extern "C" unsigned int read_battery_millivolts_svp()
{
	return OrangutanAnalog::readBatteryMillivolts_SVP();
}

#elif defined(_ORANGUTAN_X2)

extern "C" unsigned int read_battery_millivolts_x2()
{
	return OrangutanAnalog::readBatteryMillivolts_X2();
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

#endif // _ORANGUTAN_SVP


#ifdef _ORANGUTAN_SVP
static unsigned int fromMillivoltsToNormal(unsigned int millivolts);
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

static unsigned int millivolt_calibration = 5000;	// contains VCC in millivolts


// constructor
OrangutanAnalog::OrangutanAnalog()
{

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
// NOTE: Some Orangutans and 3pis have their AREF pin connected directly to VCC.
//  On these Orangutans, you must not use the internal voltage reference as
//  doing so will short the internal reference voltage to VCC and could damage
//  the AVR.  It is safe to use the internal reference voltage on the
//  Orangutan SVP.
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

	// Channel numbers greater than 31 are invalid.
	if (channel > 31)
	{
		return;
	}

	#endif

	ADCSRA = 0x87;		// bit 7 set: ADC enabled
						// bit 6 clear: don't start conversion
						// bit 5 clear: disable autotrigger
						// bit 4: ADC interrupt flag
						// bit 3 clear: disable ADC interrupt
						// bits 0-2 set: ADC clock prescaler is 128
						//  128 prescaler required for 10-bit resolution when FCPU = 20 MHz
						
	// NOTE: it is important to make changes to a temporary variable and then set the ADMUX
	// register in a single atomic operation rather than incrementally changing bits of ADMUX.
	// Specifically, setting the ADC channel by first clearing the channel bits of ADMUX and
	// then setting the ones corresponding to the desired channel briefly connects the ADC
	// to channel 0, which can affect the ADC charge capacitor.  For example, if you have a
	// high output impedance voltage on channel 1 and a low output impedance voltage on channel
	// 0, the voltage on channel 0 be briefly applied to the ADC capacitor before every conversion,
	// which could prevent the capacitor from settling to the voltage on channel 1, even over
	// many reads.
	unsigned char tempADMUX = ADMUX;

	tempADMUX |= 1 << 6;
	if(use_internal_reference)	// Note: internal reference should NOT be used on devices
	{							//  where AREF is connected to an external voltage!
		// use the internal voltage reference
		tempADMUX |= 1 << 7;		// 1.1 V on ATmega48/168/328; 2.56 V on ATmega324/644/1284
	}
	else
	{
		// use AVCC as a reference
		tempADMUX &= ~(1 << 7);
	}

	tempADMUX &= ~0x1F;		 // clear channel selection bits of ADMUX
	tempADMUX |= channel;    // we only get this far if channel is less than 32
	ADMUX = tempADMUX;
	ADCSRA |= 1 << ADSC; // start the conversion
}

// take a single analog reading of the specified channel
unsigned int OrangutanAnalog::read(unsigned char channel)
{
	startConversion(channel);
	while (isConverting());	// wait for conversion to finish
	return conversionResult();
}

// take a single analog reading of the specified channel and return the result in millivolts
unsigned int OrangutanAnalog::readMillivolts(unsigned char channel)
{
	startConversion(channel);
	while (isConverting());	// wait for conversion to finish
	return conversionResultMillivolts();
}

// take 'samples' readings of the specified channel and return the average
unsigned int OrangutanAnalog::readAverage(unsigned char channel, 
											unsigned int samples)
{
	unsigned int i = samples;
	unsigned long sum = 0;

#ifdef _ORANGUTAN_SVP
	if (channel > 31)
	{
		// We have not implemented averaging of the adc readings from the auxiliary
		// processor on the SVP, so we will just return a simple reading.
		return read(channel);
	}
#endif

	startConversion(channel);	// call this first to set the channel
	while (isConverting());		// wait while converting (discard first reading)
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


// sets the value used to calibrate the conversion from ADC reading
// to millivolts.  The argument calibration should equal VCC in millivolts,
// which can be automatically measured using the function readVCCMillivolts():
// e.g. setMillivoltCalibration(readVCCMillivolts());
void OrangutanAnalog::setMillivoltCalibration(unsigned int calibration)
{
	millivolt_calibration = calibration;
}

// averages ten ADC readings of the fixed internal 1.1V bandgap voltage
// and computes VCC from the results.  This function returns VCC in millivolts.
// Channel 14 is internal 1.1V BG on ATmega48/168/328, but bit 5 of ADMUX is
// not used, so channel 30 is equivalent to channel 14.  Channel 30 is the internal
// 1.1V BG on ATmega324/644/1284.
unsigned int OrangutanAnalog::readVCCMillivolts()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	
	// bandgap cannot deliver much current, so it takes some time for the ADC
	// to settle to the BG voltage.  The following read connects the ADC to
	// the BG voltage and gives the voltage time to settle.
	readAverage(30, 20);
	
	unsigned int reading = readAverage(30, 20);  // channel 30 is internal 1.1V BG
	unsigned int value = (1023UL * 1100UL + (reading>>1)) / reading;
	setMode(mode);
	return value;
}

// converts the specified ADC result to millivolts
unsigned int OrangutanAnalog::toMillivolts(unsigned int adcResult)
{
	unsigned long temp = adcResult * (unsigned long)millivolt_calibration;
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
		temp = (millivolts * 255UL + (millivolt_calibration>>1)) / millivolt_calibration;
		if (temp > 0xFFu)
		{
			return 0xFFu;
		}
	}
	else
	{
		temp = (millivolts * 1023UL + (millivolt_calibration>>1)) / millivolt_calibration;
		if (temp > 0xFFFFu)
		{
			return 0xFFFFu;
		}
	}
	return temp;
}

#elif defined(_ORANGUTAN_X2)

unsigned int OrangutanAnalog::readBatteryMillivolts_X2()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	unsigned int value = (readAverageMillivolts(6, 10) * 3208UL + 500) / 1000;
	setMode(mode);
	return value;
}

#else

// The temperature sensor reading (on the Orangutan LV) can be converted into degrees C as follows:
//   T = (Vout - 0.4) / 0.0195 Celcius
// The return value of this function is *tenths* of a degree Farenheit, although
// the accuracy of the temperature sensor is +/- 2 C.
int OrangutanAnalog::readTemperatureF()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	int value = (((int)(readAverageMillivolts(TEMP_SENSOR, 20)) * 12) - 634) / 13;
	setMode(mode);
	return value;
}


// Orangutan LV only: The return value of this function is *tenths* of a degree Celcius.
int OrangutanAnalog::readTemperatureC()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	int value = (((int)(readAverageMillivolts(TEMP_SENSOR, 20) * 20)) - 7982) / 39;
	setMode(mode);
	return value;
}

unsigned int OrangutanAnalog::readBatteryMillivolts_3pi()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	unsigned int value = (readAverageMillivolts(6, 10) * 3 + 1) / 2;
	setMode(mode);
	return value;
}

unsigned int OrangutanAnalog::readBatteryMillivolts_SV()
{
	unsigned char mode = getMode();
	setMode(MODE_10_BIT);
	unsigned int value = readAverageMillivolts(6,10) * 3;
	setMode(mode);
	return value;
}

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
