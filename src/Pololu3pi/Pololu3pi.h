/*
  Pololu3pi.h - Library for using the 3pi robot.
*/

/*
 * Written by Paul Grayson, 2008.
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
 
#ifndef Pololu3pi_h
#define Pololu3pi_h

#ifndef LIB_POLOLU
#ifdef NOT_ARDUINO

#include <pololu/OrangutanAnalog.h>
#include <pololu/OrangutanBuzzer.h>
#include <pololu/OrangutanMotors.h>
#include <pololu/OrangutanLCD.h>
#include <pololu/OrangutanLEDs.h>
#include <pololu/OrangutanPushbuttons.h>
#include <pololu/OrangutanTime.h>

#endif
#endif

#define IR_EMITTERS_OFF 0
#define IR_EMITTERS_ON 1
#define IR_EMITTERS_ON_AND_OFF 2

class Pololu3pi
{
public:

	// Constructor: does nothing.
	Pololu3pi() { }

	// Initializes 3pi.  Currently all that this does is set up the
	// line sensors pins, turn off the line sensor emitter LEDs to
	// conserve power, and reset the system timer.  Note: the system
	// timer is not used in the Arduino environment.
	//
	// You may choose a timeout for the line sensors, specified here
	// in microseconds.
	//
	// Setting disable_emitter_pin = 1 causes PC5 not to be used to
	// control the emitters, so that you can use it for other things.
	static void init(unsigned int line_sensor_timeout = 1000, unsigned char disable_emitter_pin = 0);

	// Reads values into the array sensor_values.  This must have
	// space for five unsigned int values and be called AFTER init.
	static void readLineSensors(unsigned int *sensor_values, unsigned char readMode = IR_EMITTERS_ON);

	void emittersOn();
	void emittersOff();
	void calibrateLineSensors(unsigned char readMode = IR_EMITTERS_ON);
	void readLineSensorsCalibrated(unsigned int *sensor_values, unsigned char readMode = IR_EMITTERS_ON);
	unsigned int readLine(unsigned int *sensor_values, unsigned char readMode = IR_EMITTERS_ON, unsigned char white_line = 0);

	unsigned int *getLineSensorsCalibratedMinimumOn();
	unsigned int *getLineSensorsCalibratedMaximumOn();
	unsigned int *getLineSensorsCalibratedMinimumOff();
	unsigned int *getLineSensorsCalibratedMaximumOff();
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
