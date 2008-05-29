#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <util/delay.h>
#include <avr/io.h>

#include "PololuQTRRC.h"

#ifdef LIB_ORANGUTAN
extern "C" void ir_leds_on()
{
	OrangutanLineSensors::emittersOn();
}

extern "C" void ir_leds_off()
{
	OrangutanLineSensors::emittersOff();
}

// returns 5 raw RC sensor values
extern "C" void read_line_sensors(unsigned int *sensor_values) {
	OrangutanLineSensors::read(sensor_values);
}
#endif

// Constructor

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
PololuQTRRC::PololuQTRRC(unsigned char* pins, unsigned char numSensors, 
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
	else if (emitterPin < 8)		// port C
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


void PololuQTRRC::read(unsigned int *sensor_values)
{
	unsigned char i;
	unsigned char startTCNT2;
	unsigned int time = 0;
	unsigned int timeout =
		(unsigned int)(_timeout_us * (unsigned long)(F_CPU / 800000UL) / 10);
	
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
	
	emittersOn();
	
	// set all ports to inputs
	DDRB &= ~_portBMask;
	DDRC &= ~_portCMask;
	DDRD &= ~_portDMask;
	
	// turn off pull ups
	PORTB &= ~_portBMask;
	PORTC &= ~_portCMask;
	PORTD &= ~_portDMask;
	
	while (time < timeout)
	{
		startTCNT2 = TCNT2;
		for (i = 0; i < _numSensors; i++)
		{
			if (sensor_values[i] == 0 && !(*_register[i] & _bitmask[i]))
				sensor_values[i] = time;
		}
		// the following line does not work without the explicit cast
		time += (unsigned char)(TCNT2 - startTCNT2);
	}

	emittersOff();
	
	for(i = 0; i < _numSensors; i++)
		if (!sensor_values[i])
			sensor_values[i] = _timeout_us;
		else
			sensor_values[i] = 
				(unsigned int)(sensor_values[i] * 10UL / 
					(unsigned char)(F_CPU / 800000UL));

}


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
