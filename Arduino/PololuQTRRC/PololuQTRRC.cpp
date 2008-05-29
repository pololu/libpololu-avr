/*
  PololuQTRRC.cpp - Library for using Pololu QTR-1RC or QTR-8RC reflectance
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

#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <util/delay.h>
#include <avr/io.h>

#include "PololuQTRRC.h"


#ifdef LIB_POLOLU
static PololuQTRRC qtr;

extern "C" void qtr_emitters_on()
{
	qtr.emittersOn();
}

extern "C" void qtr_emitters_off()
{
	qtr.emittersOff();
}

extern "C" void qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
			    unsigned int timeout_us, unsigned char emitterPin)
{
	qtr.init(pins, numSensors, timeout_us, emitterPin);
}

extern "C" void qtr_read(unsigned int *sensor_values) {
	qtr.read(sensor_values);
}

extern "C" void qtr_calibrate()
{
	qtr.calibrate();
}

extern "C" void qtr_read_calibrated(unsigned int *sensor_values)
{
	qtr.readCalibrated(sensor_values);
}

extern "C" unsigned int qtr_read_line(unsigned int *sensor_values)
{
	return qtr.readLine(sensor_values);
}

extern "C" unsigned int qtr_read_line_white(unsigned int *sensor_values)
{
	return qtr.readLine(sensor_values, true);
}

#endif

// Constructors
PololuQTRRC::PololuQTRRC()
{

}

PololuQTRRC::PololuQTRRC(unsigned char* pins, unsigned char numSensors, 
	unsigned int timeout_us, unsigned char emitterPin)
{
	init(pins, numSensors, timeout_us, emitterPin);
}

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
// ambient lighting.
// Using timeout_us allows you to shorten the
// duration of a sensor-reading cycle while still maintaining
// useful analog measurements of reflectance.

// 'emitterPin' is the Arduino pin that controls the IR LEDs on the 8RC
// modules.  If you are using a 1RC (i.e. if there is no emitter pin),
// use an invalid Arduino pin value (20 or greater).
void PololuQTRRC::init(unsigned char* pins, unsigned char numSensors, 
						 unsigned int timeout_us, unsigned char emitterPin)
{
	unsigned char i;
	_portBMask = 0;
	_portCMask = 0;
	_portDMask = 0;
	
	_timeout_us = timeout_us;
	if (numSensors > 8)
		_numSensors = 8;
	else
		_numSensors = numSensors;
	for (i = 0; i < _numSensors; i++)
	{
		// Initialize the max and min calibrated values to values that
		// will cause the first reading to update them.
		calibratedMinimum[i] = 1000;
		calibratedMaximum[i] = 0;

		if (pins[i] < 8)			// port D
		{
			_bitmask[i] = 1 << pins[i];
			_portDMask |= _bitmask[i];
			_register[i] = &PIND;
		}
		else if (pins[i] < 14)		// port B
		{
			_bitmask[i] = 1 << (pins[i] - 8);
			_portBMask |= _bitmask[i];
			_register[i] = &PINB;
		}
		else if (pins[i] < 20)		// port C
		{
			_bitmask[i] = 1 << (pins[i] - 14);
			_portCMask |= _bitmask[i];
			_register[i] = &PINC;
		}
	}
	
	if (emitterPin < 8)				// port D
	{
		_emitterBitmask = 1 << emitterPin;
		_emitterPORT = &PORTD;
		_emitterDDR = &DDRD;
	}
	else if (emitterPin < 14)		// port B
	{
		_emitterBitmask = 1 << (emitterPin - 8);
		_emitterPORT = &PORTB;
		_emitterDDR = &DDRB;
	}
	else if (emitterPin < 20)		// port C
	{
		_emitterBitmask = 1 << (emitterPin - 14);
		_emitterPORT = &PORTC;
		_emitterDDR = &DDRC;
	}
	else
	{
		_emitterDDR = 0;
		_emitterPORT = 0;
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
void PololuQTRRC::read(unsigned int *sensor_values)
{
	unsigned char i;
	unsigned char start_time;
	unsigned char delta_time;
	unsigned int time = 0;
	unsigned int timeout =
		(unsigned int)(_timeout_us * (unsigned long)(F_CPU / 800000UL) / 10);

	unsigned char last_b = _portBMask;
	unsigned char last_c = _portCMask;
	unsigned char last_d = _portDMask;
	
	TCCR2A |= 0x03;
	TCCR2B = 0x02;		// run timer2 in normal mode at 2.5 MHz
						// this is compatible with OrangutanMotors
	
	// reset the values
	for(i = 0; i < _numSensors; i++)
		sensor_values[i] = 0;
	
	// set all sensor pins to outputs
	DDRB |= _portBMask;
	DDRC |= _portCMask;
	DDRD |= _portDMask;
	
	// drive high for 10 us
	PORTB |= _portBMask;
	PORTC |= _portCMask;
	PORTD |= _portDMask;
	
	_delay_us(10);
	
	PololuQTRRC::emittersOn();
	
	// set all ports to inputs
	DDRB &= ~_portBMask;
	DDRC &= ~_portCMask;
	DDRD &= ~_portDMask;
	
	// turn off pull ups
	PORTB &= ~_portBMask;
	PORTC &= ~_portCMask;
	PORTD &= ~_portDMask;

	start_time = TCNT2;
	while (time < timeout)
	{
		// Keep track of the total time.
		// This explicitly casts the difference to unsigned char, so
		// we don't add negative values.
		delta_time = TCNT2 - start_time;
		time += delta_time;
		start_time += delta_time;

		// continue immediately if there is no change
		if (PINB == last_b && PINC == last_c && PIND == last_d)
			continue;

		// save the last observed values
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

	PololuQTRRC::emittersOff();
	
	for(i = 0; i < _numSensors; i++)
		if (!sensor_values[i])
			sensor_values[i] = _timeout_us;
		else
			sensor_values[i] = 
				(unsigned int)(sensor_values[i] * 10UL / 
					(unsigned char)(F_CPU / 800000UL));

}


// Turn the IR LEDs off and on.  This is mainly for use by the
// readLineSensors method, and calling these functions before or
// after the reading the sensors will have no effect on the
// readings, but you may wish to use these for testing purposes.
void PololuQTRRC::emittersOff()
{
	if (_emitterDDR == 0)
		return;
	*_emitterDDR |= _emitterBitmask;
	*_emitterPORT &= ~_emitterBitmask;
}


void PololuQTRRC::emittersOn()
{
  if (_emitterDDR == 0)
		return;
	*_emitterDDR |= _emitterBitmask;
	*_emitterPORT |= _emitterBitmask;
}

void PololuQTRRC::calibrate()
{
	unsigned int sensor_values[8];
	int i;
	read(sensor_values);
	
	for(i=0;i<_numSensors;i++)
	{
		if(sensor_values[i] > calibratedMaximum[i])
			calibratedMaximum[i] = sensor_values[i];
		if(sensor_values[i] < calibratedMinimum[i])
			calibratedMinimum[i] = sensor_values[i];
	}
}

void PololuQTRRC::readCalibrated(unsigned int *sensor_values)
{
	int i;

	read(sensor_values);

	for(i=0;i<_numSensors;i++)
	{
		int denominator = calibratedMaximum[i] - calibratedMinimum[i];
		signed int x = 0;
		if(denominator != 0)
			x = (((signed long)sensor_values[i]) - calibratedMinimum[i])
				* 1000 / denominator;
		if(x < 0)
			x = 0;
		else if(x > 1000)
			x = 1000;
		sensor_values[i] = x;
	}
}

unsigned int PololuQTRRC::readLine(unsigned int *sensor_values, unsigned char white_line)
{
	unsigned char i, on_line = 0;
	unsigned long avg; // this is for the weighted total, which is long
	                   // before division
	unsigned int sum; // this is for the denominator which is <= 64000
	static int16_t last_value=0; // assume initially that the line is left.

	readCalibrated(sensor_values);

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

		avg += (long)(value) * (i * 1000);
		sum += value;
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

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
