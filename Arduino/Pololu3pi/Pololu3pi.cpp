#include <avr/io.h>
#include "Pololu3pi.h"
#include "../PololuQTRRC/PololuQTRRC.h"
#include "../OrangutanAnalog/OrangutanAnalog.h"

#ifdef LIB_POLOLU

extern "C" void pololu_3pi_init(unsigned int line_sensor_timeout_us)
{
	Pololu3pi::init(line_sensor_timeout_us);
}

extern "C" unsigned int battery_millivolts()
{
	return Pololu3pi::batteryMillivolts();
}

// no read_line_sensors() definition is necessary, since the one
// defined in PololuQTRRC.cpp will work for us.

#endif

void Pololu3pi::init(unsigned int line_sensor_timeout_us)
{
	// Set up the line sensor and turn off the emitters.
	// The sensors are on PC0..4, and the emitter is on PC5.
	unsigned char pins[5] = {14,15,16,17,18};
	PololuQTRRC::init(pins,5,1000,19);
	PololuQTRRC::emittersOff();
}

void Pololu3pi::readLineSensors(unsigned int *sensor_values)
{
	return PololuQTRRC::read(sensor_values);
}

unsigned int Pololu3pi::batteryMillivolts()
{
	return OrangutanAnalog::readAverage(6,10)*5000L*3/2/1023;
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
