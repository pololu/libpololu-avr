/*
  OrangutanAnalog.h - Library for using the handling analog inputs on the
	Orangutan LV-168 and Baby Orangutan B.  This library also provides a
	method for reading the temperature sensor on the LV-168.
  Written by Ben Schmidel, May 27, 2008.
  Released into the public domain.
*/
#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#define 8_BIT_MODE
#define 10_BIT_MODE


class OrangutanAnalog
{
  public:

    // constructor (doesn't do anything)
	OrangutanAnalog();

	// set the ADC to run in either 8-bit mode (8_BIT_MODE) or 
	// 10-bit mode (10_BIT_MODE)
	static void setMode(unsigned char mode);

	// take a single analog reading of the specified channel
	static unsigned int read(unsigned char channel);
	
	// take 'sample' readings of the specified channel and return the average
	static unsigned int readAvg(unsigned char channel, unsigned int samples);
	
	// returns the position of the trimpot (4 readings averaged together).
	// The trimpot is on ADC channel 7
	static unsigned int readTrimpot();
	
	// returns the output of the LV-168's temperature sensor in tenths of a 
	// degree F or C (4 readings averaged together).  The temperature sensor 
	// is on ADC channel 6.
	static int readTemperatureF();
	static int readTemperatureC();
	
	// the following inline methods can be used to initiate an ADC conversion
	// that runs in the background, allowing the CPU to perform other tasks
	// while the conversion is in progress.  The procedure is to start a
	// conversion on a channel with startConversion(channel), and then
	// poll isConverting in your main loop.  Once isConverting() returns
	// a zero, the result can be obtained through a call to conversionResult().
	static inline void startConversion(unsigned char channel)
	{
		
	}
	
	// returns 1 if the ADC is in the middle of an conversion, otherwise
	// returns 0
	static inline unsigned char isConverting()
	{
	
	}
	
	// returns the result of the previous ADC conversion.
	static inline unsigned int conversionResult()
	{
	
	}
	
	// converts the specified ADC result to millivolts
	static unsigned int toMillivolts(unsigned int adcResult);

	
  private:
	
	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
	
	static void init2();
};

#endif

