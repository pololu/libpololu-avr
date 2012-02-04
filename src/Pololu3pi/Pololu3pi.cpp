/*
  Pololu3pi.cpp - Library for using the 3pi robot.
*/

/*
 * Written by Paul Grayson, 2008.
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
 
//#include <avr/io.h>
#include "Pololu3pi.h"

#include "../PololuQTRSensors/PololuQTRSensors.h"
#include "../OrangutanAnalog/OrangutanAnalog.h"

// a global qtr sensors
static PololuQTRSensorsRC qtr3pi;

#ifndef ARDUINO
// only needed for lib-pololu
#include "../OrangutanTime/OrangutanTime.h"
#endif

extern "C" void pololu_3pi_init(unsigned int line_sensor_timeout)
{
	Pololu3pi::init(line_sensor_timeout);
}

extern "C" void pololu_3pi_init_disable_emitter_pin(unsigned int line_sensor_timeout)
{
	Pololu3pi::init(line_sensor_timeout, 1);
}

extern "C" void read_line_sensors(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr3pi.read(sensor_values, readMode);
}

extern "C" void emitters_on()
{
	qtr3pi.emittersOn();
}

extern "C" void emitters_off()
{
	qtr3pi.emittersOff();
}

extern "C" void calibrate_line_sensors(unsigned char readMode)
{
	qtr3pi.calibrate(readMode);
}

extern "C" void line_sensors_reset_calibration()
{
	qtr3pi.resetCalibration();
}

extern "C" void read_line_sensors_calibrated(unsigned int *sensor_values, unsigned char readMode)
{
	qtr3pi.readCalibrated(sensor_values, readMode);
}

extern "C" unsigned int read_line(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr3pi.readLine(sensor_values, readMode, 0);
}

extern "C" unsigned int read_line_white(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr3pi.readLine(sensor_values, readMode, 1);
}

extern "C" unsigned int *get_line_sensors_calibrated_minimum_on()
{
	return qtr3pi.calibratedMinimumOn;
}

extern "C" unsigned int *get_line_sensors_calibrated_maximum_on()
{
	return qtr3pi.calibratedMaximumOn;
}

extern "C" unsigned int *get_line_sensors_calibrated_minimum_off()
{
	return qtr3pi.calibratedMinimumOff;
}

extern "C" unsigned int *get_line_sensors_calibrated_maximum_off()
{
	return qtr3pi.calibratedMaximumOff;
}


void Pololu3pi::init(unsigned int line_sensor_timeout_us, unsigned char disable_emitter_pin)
{
	// Set up the line sensor and turn off the emitters.
	// The sensors are on PC0..4, and the emitter is on PC5.
	unsigned char pins[5] = {14,15,16,17,18};
	if(disable_emitter_pin)
		qtr3pi.init(pins,5,line_sensor_timeout_us,255);
	else
		qtr3pi.init(pins,5,line_sensor_timeout_us,19);

	qtr3pi.emittersOff();

#ifndef ARDUINO
	// reset the time in lib-pololu mode only 
	OrangutanTime::reset();
#endif
}

void Pololu3pi::readLineSensors(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr3pi.read(sensor_values, readMode);
}

void Pololu3pi::emittersOn()
{
	qtr3pi.emittersOn();
}

void Pololu3pi::emittersOff()
{
	qtr3pi.emittersOff();
}

void Pololu3pi::calibrateLineSensors(unsigned char readMode)
{
	qtr3pi.calibrate(readMode);
}

void Pololu3pi::lineSensorsResetCalibration()
{
	qtr3pi.resetCalibration();
}

void Pololu3pi::readLineSensorsCalibrated(unsigned int *sensor_values, unsigned char readMode)
{
	qtr3pi.readCalibrated(sensor_values, readMode);
}

unsigned int Pololu3pi::readLine(unsigned int *sensor_values, unsigned char readMode, unsigned char white_line)
{
	return qtr3pi.readLine(sensor_values, readMode, white_line);
}

unsigned int *Pololu3pi::getLineSensorsCalibratedMinimumOn()
{
	return qtr3pi.calibratedMinimumOn;
}

unsigned int *Pololu3pi::getLineSensorsCalibratedMaximumOn()
{
	return qtr3pi.calibratedMaximumOn;
}

unsigned int *Pololu3pi::getLineSensorsCalibratedMinimumOff()
{
	return qtr3pi.calibratedMinimumOff;
}

unsigned int *Pololu3pi::getLineSensorsCalibratedMaximumOff()
{
	return qtr3pi.calibratedMaximumOff;
}



// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
