/*
  OrangutanAnalog.h - Library for using the handling analog inputs on the
	Orangutan LV-168 and Baby Orangutan B.  This library also provides a
	method for reading the temperature sensor on the LV-168.
  Written by Ben Schmidel, May 27, 2008.
  http://www.pololu.com
  http://forum.pololu.com
  Released into the public domain.
  
  Disclaimer: To the extent permitted by law, Pololu Corporation provides
  this work without any warranty.  It may be defective, in which case you
  agree to be responsible for all resulting costs and damages.
*/

#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#define MODE_8_BIT		1
#define MODE_10_BIT		0
#define TRIMPOT			7
#define TEMP_SENSOR		6


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
		return ADMUX & (1 << ADLAR) ? 1 : 0;
	}

	// take a single analog reading of the specified channel
	static unsigned int read(unsigned char channel);
	
	// take 'sample' readings of the specified channel and return the average
	static unsigned int readAverage(unsigned char channel, 
									  unsigned int samples);
	
	// returns the position of the trimpot (20 readings averaged together).
	// The trimpot is on ADC channel 7
	static unsigned int readTrimpot();
	
	// returns the output of the LV-168's temperature sensor in tenths of a 
	// degree F or C (20 readings averaged together).  The temperature sensor 
	// is on ADC channel 6.
	static int readTemperatureF();
	static int readTemperatureC();
	
	// the following methods can be used to initiate an ADC conversion
	// that runs in the background, allowing the CPU to perform other tasks
	// while the conversion is in progress.  The procedure is to start a
	// conversion on a channel with startConversion(channel), and then
	// poll isConverting in your main loop.  Once isConverting() returns
	// a zero, the result can be obtained through a call to conversionResult().
	static void startConversion(unsigned char channel);

	// returns 1 if the ADC is in the middle of an conversion, otherwise
	// returns 0
	static inline unsigned char isConverting()
	{
		return ADCSRA & (1 << ADSC);
	}
	
	// returns the result of the previous ADC conversion.
	static inline unsigned int conversionResult()
	{
		if (getMode())				// if left-adjusted (i.e. 8-bit mode)
			return ADCH;			// 8-bit result
		return ADC;				// 10-bit result
	}
	
	// converts the specified ADC result to millivolts
	static unsigned int toMillivolts(unsigned int adcResult);
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
