/*
  PololuQTRRC.h - Library for using Pololu QTR-1RC or QTR-8RC reflectance
	sensors.  Simply specify in the constructor which Arduino I/O pins are
	connected to a QTR-RC sensor, and then the read() method will obtain
	reflectance measurements for those sensors.  The sensor values returned
	are pulse lengths measured in microseconds, with shorter pulses
	corresponding to higher reflectance.
  Written by Ben Schmidel and Paul Grayson, May 28, 2008.
  http://www.pololu.com
  http://forum.pololu.com
  Released into the public domain.
  
  Disclaimer: To the extent permitted by law, Pololu Corporation provides
  this work without any warranty.  It may be defective, in which case you
  agree to be responsible for all resulting costs and damages.
*/

#ifndef PololuQTRRC_h
#define PololuQTRRC_h

class PololuQTRRC
{
  public:

	// Constructor
	PololuQTRRC();
	PololuQTRRC(unsigned char* pins, unsigned char numSensors, 
		  unsigned int timeout_us, unsigned char emitterPin);

	// the array 'pins' contains the Arduino pin assignment for each
	// sensor.  For example, if pins is {3, 6, 15}, sensor 1 is on
	// Arduino digital pin 3, sensor 2 is on Arduino Digital pin 6,
	// and sensor 3 is on Arduino analog input 1 (digital pin 15).
	// Note that Arduino digital pins 0 - 7 correpsond to port D
	// pins PD0 - PD7, respectively.  Arduino digital pins 8 - 13
	// correspond to port B pins PB0 - PB5.  Arduino analog inputs
	// 0 - 5 are referred to as digital pins 14 - 19 (these are the
	// enumerations you should use for this library) and correspond
	// to port C pins PC0 - PC5.
	
	// 'numSensors' specifies the length of the 'pins' array (i.e. the
	// number of QTR-RC sensors you are using).  numSensors must be 
	// no greater than 8.
	
	// 'timeout_us' specifies the length of time in microseconds beyond
	// which you consider the sensor reading completely black.  That is to say,
	// if the pulse length for a pin exceeds timeout_us, pulse timing will stop
	// and the reading for that pin will be considered full black.
	// It is recommended that you set timeout_us to be between 1000 and
	// 3000, depending on things like the height of your sensors and
	// ambient lighting.  It *MUST* be less than 6000.
	// Using timeout_us allows you to shorten the
	// duration of a sensor-reading cycle while still maintaining
	// useful analog measurements of reflectance.
	void init(unsigned char* pins, unsigned char numSensors, 
		  unsigned int timeout_us, unsigned char emitterPin);
	
	// Reads the sensor values into an array. There *MUST* be space
	// for as many values as there were sensors specified in the constructor.
	// Example usage:
	// unsigned int sensor_values[8];
	// sensors.read(sensor_values);
	// ...
	// The values returned are in microseconds and range from 0 to
	// timeout_us (as specified in the constructor).
	void read(unsigned int *sensor_values);
	
	// Turn the IR LEDs off and on.  This is mainly for use by the
	// readLineSensors method, and calling these functions before or
	// after the reading the sensors will have no effect on the
	// readings, but you may wish to use these for testing purposes.
	void emittersOff();
	void emittersOn();
  
  
  private:
  
	unsigned char _bitmask[8];
	// pointer to PIN registers
	volatile unsigned char* _register[8];	// needs to be volatile
	unsigned char _numSensors;
	unsigned int _timeout_us;
	unsigned char _emitterBitmask;
	// pointer to emitter PORT register
	volatile unsigned char* _emitterPORT;	// needs to be volatile
	// pointer to emitter DDR register
	volatile unsigned char* _emitterDDR;		// needs to be volatile
		
	unsigned char _portBMask;
	unsigned char _portCMask;
	unsigned char _portDMask;
  
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
