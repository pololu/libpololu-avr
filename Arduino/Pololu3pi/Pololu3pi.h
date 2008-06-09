#ifndef Pololu3pi_h
#define Pololu3pi_h

class Pololu3pi
{
public:

	// Constructor: does nothing.
	Pololu3pi() { }

	// Initializes 3pi.  Currently all that this does is set up the
	// line sensors pins and turn off the line sensor emitter LEDs to
	// conserve power.
	//
	// You may choose a timeout for the line sensors, specified here
	// in microseconds.
	static void init(unsigned int line_sensor_timeout_us = 1000);

	// Reads values into the array sensor_values.  This must have
	// space for five unsigned int values and be called AFTER init.
	static void readLineSensors(unsigned int *sensor_values, unsigned char readMode);

	void emittersOn();
	void emittersOff();
	void calibrateLineSensors(unsigned char readMode);
	void readLineSensorsCalibrated(unsigned int *sensor_values, unsigned char readMode);
	unsigned int readLine(unsigned int *sensor_values, unsigned char readMode, unsigned char white_line);

	// Returns the voltage of the battery in millivolts, using 10
	// averaged samples.
	static unsigned int batteryMillivolts();
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
