/*
  PololuQTRSensors.cpp - Library for using Pololu QTR reflectance
	sensors and reflectance sensor arrays: QTR-1A, QTR-8A, QTR-1RC, and 
	QTR-8RC.  The object used will determine the type of the sensor (either
	QTR-xA or QTR-xRC).  Then simply specify in the constructor which 
	Arduino I/O pins are connected to a QTR sensor, and the read() method
	will obtain reflectance measurements for those sensors.  Smaller sensor
	values correspond to higher reflectance (e.g. white) while larger
	sensor values correspond to lower reflectance (e.g. black or a void).
	
	* PololuQTRSensorsRC should be used for QTR-1RC and QTR-8RC sensors.
	* PololuQTRSensorsAnalog should be used for QTR-1A and QTR-8A sensors.
*/
	
/*
 * Written by Ben Schmidel et al., June 4, 2010.
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <avr/io.h>
#include <stdlib.h>
#include "PololuQTRSensors.h"

#ifdef _ORANGUTAN_XX4
  #define ANALOG_PORT PORTA
  #define ANALOG_DDR  DDRA
#else
  #define ANALOG_PORT PORTC
  #define ANALOG_DDR  DDRC
#endif

#define QTR_RC		0
#define QTR_A		1

#include "../OrangutanDigital/OrangutanDigital.h" // provides pin definitions

#ifndef ARDUINO
#include "../OrangutanTime/OrangutanTime.h"		// provides access to delay routines
#else
#include <Arduino.h> // provides access to delay() and delayMicroseconds()
#endif

// one pointer to the type in use
static PololuQTRSensors *qtr;

extern "C" void qtr_emitters_on()
{
	qtr->emittersOn();
}

extern "C" void qtr_emitters_off()
{
	qtr->emittersOff();
}

extern "C" char qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
			    unsigned int timeout, unsigned char emitterPin)
{
	PololuQTRSensorsRC *qtr_rc = (PololuQTRSensorsRC *)malloc(sizeof(PololuQTRSensorsRC));
	if(!qtr_rc)
		return 0; // out of memory
	qtr_rc->init(pins, numSensors, timeout, emitterPin);
	qtr = qtr_rc;
	return 1;
}

extern "C" char qtr_analog_init(unsigned char* analogPins, unsigned char numSensors, 
		unsigned char numSamplesPerSensor, unsigned char emitterPin)
{
	PololuQTRSensorsAnalog *qtr_analog = (PololuQTRSensorsAnalog *)malloc(sizeof(PololuQTRSensorsAnalog));
	if(!qtr_analog)
		return 0; // out of memory
	qtr_analog->init(analogPins, numSensors, numSamplesPerSensor, emitterPin);
	qtr = qtr_analog;
	return 1;
}

extern "C" void qtr_read(unsigned int *sensor_values, unsigned char readMode) {
	qtr->read(sensor_values,readMode);
}

extern "C" void qtr_calibrate(unsigned char readMode)
{
	qtr->calibrate(readMode);
}

extern "C" void qtr_read_calibrated(unsigned int *sensor_values, unsigned char readMode)
{
	qtr->readCalibrated(sensor_values, readMode);
}

extern "C" unsigned int qtr_read_line(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr->readLine(sensor_values, readMode, false);
}

extern "C" unsigned int qtr_read_line_white(unsigned int *sensor_values, unsigned char readMode)
{
	return qtr->readLine(sensor_values, readMode, true);
}

extern "C" void qtr_reset_calibration()
{
	qtr->resetCalibration();
}

extern "C" unsigned int *qtr_calibrated_minimum_on()
{
	return qtr->calibratedMinimumOn;
}

extern "C" unsigned int *qtr_calibrated_maximum_on()
{
	return qtr->calibratedMaximumOn;
}

extern "C" unsigned int *qtr_calibrated_minimum_off()
{
	return qtr->calibratedMinimumOff;
}

extern "C" unsigned int *qtr_calibrated_maximum_off()
{
	return qtr->calibratedMaximumOff;
}


// Base class data member initialization (called by derived class init())
void PololuQTRSensors::init(unsigned char numSensors, 
  unsigned char emitterPin, unsigned char type)
{
	calibratedMinimumOn=0;
	calibratedMaximumOn=0;
	calibratedMinimumOff=0;
	calibratedMaximumOff=0;

	if (numSensors > QTR_MAX_SENSORS)
		_numSensors = QTR_MAX_SENSORS;
	else
		_numSensors = numSensors;
		
	_type = type;

	struct IOStruct emitterIO;
	OrangutanDigital::getIORegisters(&emitterIO, emitterPin);
    _emitterBitmask = emitterIO.bitmask;
    _emitterPORT = emitterIO.portRegister;
    _emitterDDR = emitterIO.ddrRegister;
}


// Reads the sensor values into an array. There *MUST* be space
// for as many values as there were sensors specified in the constructor.
// Example usage:
// unsigned int sensor_values[8];
// sensors.read(sensor_values);
// The values returned are a measure of the reflectance in abstract units,
// with higher values corresponding to lower reflectance (e.g. a black
// surface or a void).
void PololuQTRSensors::read(unsigned int *sensor_values, unsigned char readMode)
{
	unsigned int off_values[QTR_MAX_SENSORS];
	unsigned char i;
	
	if(readMode == QTR_EMITTERS_ON || readMode == QTR_EMITTERS_ON_AND_OFF)
		emittersOn();

	if (_type == QTR_RC)
	{
		((PololuQTRSensorsRC*)this)->readPrivate(sensor_values);
		emittersOff();
		if(readMode == QTR_EMITTERS_ON_AND_OFF)
			((PololuQTRSensorsRC*)this)->readPrivate(off_values);
	}
	else
	{
		((PololuQTRSensorsAnalog*)this)->readPrivate(sensor_values);
		emittersOff();
		if(readMode == QTR_EMITTERS_ON_AND_OFF)
			((PololuQTRSensorsAnalog*)this)->readPrivate(off_values);
	}

	if(readMode == QTR_EMITTERS_ON_AND_OFF)
	{
		for(i=0;i<_numSensors;i++)
		{
			sensor_values[i] += _maxValue - off_values[i];
		}
	}
}


// Turn the IR LEDs off and on.  This is mainly for use by the
// read method, and calling these functions before or
// after the reading the sensors will have no effect on the
// readings, but you may wish to use these for testing purposes.
void PololuQTRSensors::emittersOff()
{
	if (_emitterDDR == 0)
		return;
	*_emitterDDR |= _emitterBitmask;
	*_emitterPORT &= ~_emitterBitmask;

	delayMicroseconds(200);  // Give the sensors time to react.
}

void PololuQTRSensors::emittersOn()
{
  if (_emitterDDR == 0)
		return;
	*_emitterDDR |= _emitterBitmask;
	*_emitterPORT |= _emitterBitmask;

	delayMicroseconds(200);  // Give the sensors time to react.
}

// Resets the calibration.
void PololuQTRSensors::resetCalibration()
{
	unsigned char i;
	for(i=0;i<_numSensors;i++)
	{
		if(calibratedMinimumOn)
			calibratedMinimumOn[i] = _maxValue;
		if(calibratedMinimumOff)
			calibratedMinimumOff[i] = _maxValue;
		if(calibratedMaximumOn)
			calibratedMaximumOn[i] = 0;
		if(calibratedMaximumOff)
			calibratedMaximumOff[i] = 0;
	}
}

// Reads the sensors 10 times and uses the results for
// calibration.  The sensor values are not returned; instead, the
// maximum and minimum values found over time are stored internally
// and used for the readCalibrated() method.
void PololuQTRSensors::calibrate(unsigned char readMode)
{
	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_ON)
	{
		calibrateOnOrOff(&calibratedMinimumOn,
						 &calibratedMaximumOn,
						 QTR_EMITTERS_ON);
	}


	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_OFF)
	{
		calibrateOnOrOff(&calibratedMinimumOff,
						 &calibratedMaximumOff,
						 QTR_EMITTERS_OFF);
	}
}

void PololuQTRSensors::calibrateOnOrOff(unsigned int **calibratedMinimum,
										unsigned int **calibratedMaximum,
										unsigned char readMode)
{
	int i;
	unsigned int sensor_values[16];
	unsigned int max_sensor_values[16];
	unsigned int min_sensor_values[16];

	// Allocate the arrays if necessary.
	if(*calibratedMaximum == 0)
	{
		*calibratedMaximum = (unsigned int*)malloc(sizeof(unsigned int)*_numSensors);

		// If the malloc failed, don't continue.
		if(*calibratedMaximum == 0)
			return;

		// Initialize the max and min calibrated values to values that
		// will cause the first reading to update them.

		for(i=0;i<_numSensors;i++)
			(*calibratedMaximum)[i] = 0;
	}
	if(*calibratedMinimum == 0)
	{
		*calibratedMinimum = (unsigned int*)malloc(sizeof(unsigned int)*_numSensors);

		// If the malloc failed, don't continue.
		if(*calibratedMinimum == 0)
			return;

		for(i=0;i<_numSensors;i++)
			(*calibratedMinimum)[i] = _maxValue;
	}

	int j;
	for(j=0;j<10;j++)
	{
		read(sensor_values,readMode);
		for(i=0;i<_numSensors;i++)
		{
			// set the max we found THIS time
			if(j == 0 || max_sensor_values[i] < sensor_values[i])
				max_sensor_values[i] = sensor_values[i];

			// set the min we found THIS time
			if(j == 0 || min_sensor_values[i] > sensor_values[i])
				min_sensor_values[i] = sensor_values[i];
		}
	}

	// record the min and max calibration values
	for(i=0;i<_numSensors;i++)
	{
		if(min_sensor_values[i] > (*calibratedMaximum)[i])
			(*calibratedMaximum)[i] = min_sensor_values[i];
		if(max_sensor_values[i] < (*calibratedMinimum)[i])
			(*calibratedMinimum)[i] = max_sensor_values[i];
	}
}


// Returns values calibrated to a value between 0 and 1000, where
// 0 corresponds to the minimum value read by calibrate() and 1000
// corresponds to the maximum value.  Calibration values are
// stored separately for each sensor, so that differences in the
// sensors are accounted for automatically.
void PololuQTRSensors::readCalibrated(unsigned int *sensor_values, unsigned char readMode)
{
	int i;

	// if not calibrated, do nothing
	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_OFF)
		if(!calibratedMinimumOff || !calibratedMaximumOff)
			return;
	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_ON)
		if(!calibratedMinimumOn || !calibratedMaximumOn)
			return;

	// read the needed values
	read(sensor_values,readMode);

	for(i=0;i<_numSensors;i++)
	{
		unsigned int calmin,calmax;
		unsigned int denominator;

		// find the correct calibration
		if(readMode == QTR_EMITTERS_ON)
		{
			calmax = calibratedMaximumOn[i];
		    calmin = calibratedMinimumOn[i];
		}
		else if(readMode == QTR_EMITTERS_OFF)
		{
			calmax = calibratedMaximumOff[i];
		    calmin = calibratedMinimumOff[i];
		}
		else // QTR_EMITTERS_ON_AND_OFF
		{
			
			if(calibratedMinimumOff[i] < calibratedMinimumOn[i]) // no meaningful signal
				calmin = _maxValue;
			else
				calmin = calibratedMinimumOn[i] + _maxValue - calibratedMinimumOff[i]; // this won't go past _maxValue

			if(calibratedMaximumOff[i] < calibratedMaximumOn[i]) // no meaningful signal
				calmax = _maxValue;
			else
				calmax = calibratedMaximumOn[i] + _maxValue - calibratedMaximumOff[i]; // this won't go past _maxValue
		}

		denominator = calmax - calmin;

		signed int x = 0;
		if(denominator != 0)
			x = (((signed long)sensor_values[i]) - calmin)
				* 1000 / denominator;
		if(x < 0)
			x = 0;
		else if(x > 1000)
			x = 1000;
		sensor_values[i] = x;
	}

}


// Operates the same as read calibrated, but also returns an
// estimated position of the robot with respect to a line. The
// estimate is made using a weighted average of the sensor indices
// multiplied by 1000, so that a return value of 0 indicates that
// the line is directly below sensor 0, a return value of 1000
// indicates that the line is directly below sensor 1, 2000
// indicates that it's below sensor 2000, etc.  Intermediate
// values indicate that the line is between two sensors.  The
// formula is:
// 
//    0*value0 + 1000*value1 + 2000*value2 + ...
//   --------------------------------------------
//         value0  +  value1  +  value2 + ...
//
// By default, this function assumes a dark line (high values)
// surrounded by white (low values).  If your line is light on
// black, set the optional second argument white_line to true.  In
// this case, each sensor value will be replaced by (1000-value)
// before the averaging.
unsigned int PololuQTRSensors::readLine(unsigned int *sensor_values,
	unsigned char readMode, unsigned char white_line)
{
	unsigned char i, on_line = 0;
	unsigned long avg; // this is for the weighted total, which is long
	                   // before division
	unsigned int sum; // this is for the denominator which is <= 64000
	static int last_value=0; // assume initially that the line is left.

	readCalibrated(sensor_values, readMode);

	avg = 0;
	sum = 0;
  
	for(i=0;i<_numSensors;i++) {
		int value = sensor_values[i];
		if(white_line)
			value = 1000-value;

		// keep track of whether we see the line at all
		if(value > 200) {
			on_line = 1;
		}
		
		// only average in values that are above a noise threshold
		if(value > 50) {
			avg += (long)(value) * (i * 1000);
			sum += value;
		}
	}

	if(!on_line)
	{
		// If it last read to the left of center, return 0.
		if(last_value < (_numSensors-1)*1000/2)
			return 0;
		
		// If it last read to the right of center, return the max.
		else
			return (_numSensors-1)*1000;

	}

	last_value = avg/sum;

	return last_value;
}



// Derived RC class constructor
PololuQTRSensorsRC::PololuQTRSensorsRC(unsigned char* pins,
  unsigned char numSensors, unsigned int timeout, unsigned char emitterPin)
{
	init(pins, numSensors, timeout, emitterPin);
}


// the array 'pins' contains the pin number for each sensor,
// as defined in the IO_* macros in OrangutanDigital.h.
// For ATmega48/168/328 processors, the pin numbers correspond
// to the Arduino pin numbers.
// For example, if pins is {3, 6, 15}, sensor 1 is on
// Arduino digital pin 3, sensor 2 is on Arduino digital pin 6,
// and sensor 3 is on Arduino analog input 1 (digital pin 15).
// Note that Arduino digital pins 0 - 7 correpsond to port D
// pins PD0 - PD7, respectively.  Arduino digital pins 8 - 13
// correspond to port B pins PB0 - PB5.  Arduino analog inputs
// 0 - 5 are referred to as digital pins 14 - 19 (these are the
// enumerations you should use for this library) and correspond
// to port C pins PC0 - PC5.

// 'numSensors' specifies the length of the 'pins' array (i.e. the
// number of QTR-RC sensors you are using).  numSensors must be 
// no greater than 16.

// 'timeout' specifies the length of time in timer2 counts beyond
// which you consider the sensor reading completely black.  That is to say,
// if the pulse length for a pin exceeds 'timeout', pulse timing will stop
// and the reading for that pin will be considered full black.
// It is recommended that you set timeout to be between 1000 and
// 3000 us, depending on things like the height of your sensors and
// ambient lighting.  Using timeout allows you to shorten the
// duration of a sensor-reading cycle while still maintaining
// useful analog measurements of reflectance.  On a 16 MHz microcontroller,
// you can convert timer2 counts to microseconds by dividing by 2
// (i.e. 2000 us = 4000 timer2 counts = 'timeout' of 4000).  On a 20 MHz
// microcontroller, you can convert timer2 counts to microseconds by
// dividing by 2.5 or multiplying by 0.4 
// (i.e. 2000 us = 5000 timer2 counts = 'timeout' of 5000).
	
// 'emitterPin' is the Arduino pin that controls the IR LEDs on the 8RC
// modules.  If you are using a 1RC (i.e. if there is no emitter pin),
// use an invalid Arduino pin value (20 or greater).
void PololuQTRSensorsRC::init(unsigned char* pins,
	unsigned char numSensors, unsigned int timeout, unsigned char emitterPin)
{
	PololuQTRSensors::init(numSensors, emitterPin, QTR_RC);
	
	unsigned char i;
    #ifdef _ORANGUTAN_XX4
	_portAMask = 0;
    #endif
	_portBMask = 0;
	_portCMask = 0;
	_portDMask = 0;
	
	_maxValue = timeout;
	for (i = 0; i < _numSensors; i++)
	{
		struct IOStruct sensorIO;
		OrangutanDigital::getIORegisters(&sensorIO, pins[i]);
		_bitmask[i] = sensorIO.bitmask;
		_register[i] = sensorIO.pinRegister;
        #ifdef _ORANGUTAN_XX4
		if (sensorIO.pinRegister == &PINA) { _portAMask |= sensorIO.bitmask; }
		#endif
		if (sensorIO.pinRegister == &PINB) { _portBMask |= sensorIO.bitmask; }
		if (sensorIO.pinRegister == &PINC) { _portCMask |= sensorIO.bitmask; }
		if (sensorIO.pinRegister == &PIND) { _portDMask |= sensorIO.bitmask; }
	}
}


// Reads the sensor values into an array. There *MUST* be space
// for as many values as there were sensors specified in the constructor.
// Example usage:
// unsigned int sensor_values[8];
// sensors.read(sensor_values);
// ...
// The values returned are in microseconds and range from 0 to
// timeout_us (as specified in the constructor).
void PololuQTRSensorsRC::readPrivate(unsigned int *sensor_values)
{
	unsigned char i;
	unsigned char last_time;
	unsigned char delta_time;
	unsigned int time = 0;

	#ifdef _ORANGUTAN_XX4
	unsigned char last_a = _portAMask;
    #endif
	unsigned char last_b = _portBMask;
	unsigned char last_c = _portCMask;
	unsigned char last_d = _portDMask;
	
	// reset the values
	for(i = 0; i < _numSensors; i++)
		sensor_values[i] = 0;
		
	// set all sensor pins to outputs
	#ifdef _ORANGUTAN_XX4
	DDRA |= _portAMask;
	#endif
	DDRB |= _portBMask;
	DDRC |= _portCMask;
	DDRD |= _portDMask;
	
	// drive high for 10 us
	#ifdef _ORANGUTAN_XX4
	PORTA |= _portAMask;
	#endif
	PORTB |= _portBMask;
	PORTC |= _portCMask;
	PORTD |= _portDMask;
	
	delayMicroseconds(10);
	
	// set all ports to inputs
	#ifdef _ORANGUTAN_XX4
	DDRA &= ~_portAMask;
	#endif
	DDRB &= ~_portBMask;
	DDRC &= ~_portCMask;
	DDRD &= ~_portDMask;
	
	// turn off pull ups
    #ifdef _ORANGUTAN_XX4
	PORTA &= ~_portBMask;
	#endif
	PORTB &= ~_portBMask;
	PORTC &= ~_portCMask;
	PORTD &= ~_portDMask;

	unsigned char prevTCCR2A = TCCR2A;
	unsigned char prevTCCR2B = TCCR2B;
	TCCR2A |= 0x03;
	TCCR2B = 0x02;		// run timer2 in normal mode at 2.5 MHz
						// this is compatible with OrangutanMotors

	last_time = TCNT2;
	while (time < _maxValue)
	{
		// Keep track of the total time.
		// This implicitly casts the difference to unsigned char, so
		// we don't add negative values.
		delta_time = TCNT2 - last_time;
		time += delta_time;
		last_time += delta_time;

		// continue immediately if there is no change
        #ifdef _ORANGUTAN_XX4
		if (PINA == last_a && PINB == last_b && PINC == last_c && PIND == last_d) continue;
		#else
		if (PINB == last_b && PINC == last_c && PIND == last_d) continue;
        #endif

		// save the last observed values
		#ifdef _ORANGUTAN_XX4
		last_a = PINA;
		#endif
		last_b = PINB;
		last_c = PINC;
		last_d = PIND;

		// figure out which pins changed
		for (i = 0; i < _numSensors; i++)
		{
			if (sensor_values[i] == 0 && !(*_register[i] & _bitmask[i]))
				sensor_values[i] = time;
		}
	}

	TCCR2A = prevTCCR2A;
	TCCR2B = prevTCCR2B;
	for(i = 0; i < _numSensors; i++)
		if (!sensor_values[i])
			sensor_values[i] = _maxValue;
}



// Derived Analog class constructor
PololuQTRSensorsAnalog::PololuQTRSensorsAnalog(unsigned char* analogPins,
  unsigned char numSensors, unsigned char numSamplesPerSensor,
  unsigned char emitterPin)
{
	init(analogPins, numSensors, numSamplesPerSensor, emitterPin);
}


// the array 'pins' contains the Arduino analog pin assignment for each
// sensor.  For example, if pins is {0, 1, 7}, sensor 1 is on
// Arduino analog input 0, sensor 2 is on Arduino analog input 1,
// and sensor 3 is on Arduino analog input 7.  The ATmega168/328 has 8
// total analog input channels (0 - 7) that correspond to port C
// pins PC0 - PC7.  The ATmega324/644/1284 has 8 total analog input
// channels (0-7) that correspond to port A pina PA0 - PA7.

// 'numSensors' specifies the length of the 'analogPins' array (i.e. the
// number of QTR-A sensors you are using).  numSensors must be 
// no greater than 16.

// 'numSamplesPerSensor' indicates the number of 10-bit analog samples
// to average per channel (i.e. per sensor) for each reading.  The total
// number of analog-to-digital conversions performed will be equal to
// numSensors*numSamplesPerSensor.  Note that the amount of time it takes
// to perform a single analog-to-digital conversion is approximately:
// 128 * 13 / F_CPU = 1664 / F_CPU
// If F_CPU is 16 MHz, as on most Arduinos, this becomes:
// 1664 / 16 MHz = 104 us
// So if numSamplesPerSensor is 4 and numSensors is, say, 6, it will take
// 4 * 6 * 104 us = 2.5 ms to perform a full readLine() if F_CPU is 16 MHz.
// Increasing this parameter increases noise suppression at the cost of
// sample rate.  Recommended value: 4.

// emitterPin is the Arduino digital pin that controls whether the IR LEDs
// are on or off.  This pin is optional and only exists on the 8A and 8RC
// QTR sensor arrays.  If a valid pin is specified, the emitters will only
// be turned on during a reading.  If an invalid pin is specified 
// (e.g. 255), the IR emitters will always be on.
void PololuQTRSensorsAnalog::init(unsigned char* analogPins,
	unsigned char numSensors, unsigned char numSamplesPerSensor,
	unsigned char emitterPin)
{
	unsigned char i;
	
	PololuQTRSensors::init(numSensors, emitterPin, QTR_A);
	
	_numSamplesPerSensor = numSamplesPerSensor;
	_portMask = 0;
	for (i = 0; i < _numSensors; i++)
	{
		_analogPins[i] = analogPins[i];
    	_portMask |= (1 << analogPins[i]);
	}

	#ifndef _ORANGUTAN_XX4
	// no need to mask for dedicated analog inputs ADC6 and ADC7
	_portMask &= 0x3F;
	#endif

	_maxValue = 1023; // this is the maximum returned by the A/D conversion
}


// Reads the sensor values into an array. There *MUST* be space
// for as many values as there were sensors specified in the constructor.
// Example usage:
// unsigned int sensor_values[8];
// sensors.read(sensor_values);
// The values returned are a measure of the reflectance in terms of a
// 10-bit ADC average with higher values corresponding to lower 
// reflectance (e.g. a black surface or a void).
void PololuQTRSensorsAnalog::readPrivate(unsigned int *sensor_values)
{
	unsigned char i, j;
	
	// store current state of various registers
	unsigned char admux = ADMUX;
	unsigned char adcsra = ADCSRA;
	unsigned char ddr = ANALOG_DDR;
	unsigned char port = ANALOG_PORT;

	// wait for any current conversion to finish
	while (ADCSRA & (1 << ADSC));
	
	// reset the values
	for(i = 0; i < _numSensors; i++)
		sensor_values[i] = 0;

	// set all sensor pins to high-Z inputs
	ANALOG_DDR &= ~_portMask;
	ANALOG_PORT &= ~_portMask;

	ADCSRA = 0x87;	// configure the ADC
	for (j = 0; j < _numSamplesPerSensor; j++)
	{
		for (i = 0; i < _numSensors; i++)
		{
			ADMUX = (1<<6) | _analogPins[i];// set analog input channel
			ADCSRA |= 1 << ADSC;			// start the conversion
			while (ADCSRA & (1 << ADSC));	// wait for conversion to finish
			sensor_values[i] += ADC;		// add in the conversion result
		}
	}
	
	// get the rounded average of the readings for each sensor
	for (i = 0; i < _numSensors; i++)
		sensor_values[i] = (sensor_values[i] + (_numSamplesPerSensor >> 1)) /
			_numSamplesPerSensor;

	ADMUX = admux;
	ADCSRA = adcsra;
	ANALOG_PORT = port;
	ANALOG_DDR = ddr;
}

// the destructor frees up allocated memory
PololuQTRSensors::~PololuQTRSensors()
{
	if(calibratedMaximumOn)
		free(calibratedMaximumOn);
	if(calibratedMaximumOff)
		free(calibratedMaximumOff);
	if(calibratedMinimumOn)
		free(calibratedMinimumOn);
	if(calibratedMinimumOff)
		free(calibratedMinimumOff);
}


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
