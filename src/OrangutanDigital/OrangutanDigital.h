/*
  OrangutanDigital.h - Library for using the digital I/O lines on the
	Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  The code
	is all inline, which lets it compile to very small, fast, efficient
	assembly code if you use constants as your inputs.  For example,
	the line:
	
		setOutput(3, HIGH);
	
	compiles to the assembly:
	
		sbi 0x0b, 3  ;i.e. PORTD |= 1 << 3;
		sbi 0x0a, 3  ;i.e. DDRD  |= 1 << 3;
	
	In short, if your inputs are constants, you can use this library in
	place of raw digital I/O register manipulation without worrying
	about any significantly increased overhead or processing time.
	Using variables as inputs can increase overhead and processing time,
	but the functions in this library allow for simpler programmatic
	approaches to working with digital I/O, since you no longer have to
	deal with a multitude of pin-specific registers.
	
	The digital pins on the AVR default to high-impedance inputs after
	a power-up or reset.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
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


#ifndef OrangutanDigital_h
#define OrangutanDigital_h

#include "../OrangutanResources/include/OrangutanModel.h"
#include <avr/io.h>

#define INPUT 				0
#define OUTPUT				1
#define LOW					0
#define HIGH				1
#define TOGGLE				0xFF
#define HIGH_IMPEDANCE		0
#define PULL_UP_ENABLED		1

// port D pins
#define IO_D0				0
#define IO_D1				1
#define IO_D2				2
#define IO_D3				3
#define IO_D4				4
#define IO_D5				5
#define IO_D6				6
#define IO_D7				7

// port B pins
#define IO_B0				8
#define IO_B1				9
#define IO_B2				10
#define IO_B3				11
#define IO_B4				12
#define IO_B5				13


#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)

#define IO_B6				14
#define IO_B7				15

// port C pins
#define IO_C0				16
#define IO_C1				17
#define IO_C2				18
#define IO_C3				19
#define IO_C4				20
#define IO_C5				21
#define IO_C6				22
#define IO_C7				23

// port A pins
#define IO_A0				31
#define IO_A1				30
#define IO_A2				29
#define IO_A3				28
#define IO_A4				27
#define IO_A5				26
#define IO_A6				25
#define IO_A7				24

#else

// port C pins
#define IO_C0				14
#define IO_C1				15
#define IO_C2				16
#define IO_C3				17
#define IO_C4				18
#define IO_C5				19
#define IO_C6				20	// only used if RESET pin is changed to be a digital I/O

#endif


struct IOStruct
{
	// if these aren't volatile, the compiler sometimes incorrectly optimizes away operations involving these registers:
	volatile unsigned char* pinRegister;
	volatile unsigned char* portRegister;
	volatile unsigned char* ddrRegister;
	unsigned char bitmask;
};

#ifdef __cplusplus

class OrangutanDigital
{
  public:

    // constructor (doesn't do anything)
	OrangutanDigital();
	
	
	// gets a structure with pointers to the three digital I/O registers associated
	// with the specified pin (DDR, PORT, and PIN) along with a bitmask with a
	// 1 in the position of the specified pin and 0s everywhere else.
	inline static void getIORegisters(struct IOStruct* io, unsigned char pin)
	{
		io->pinRegister = 0;
		io->portRegister = 0;
		io->ddrRegister = 0;
		io->bitmask = 0;

		if (pin < 8)			// pin 0 = PD0, ..., 7 = PD7
		{
			io->pinRegister = (unsigned char*)&PIND;
			io->portRegister = (unsigned char*)&PORTD;
			io->ddrRegister = (unsigned char*)&DDRD;
			io->bitmask = 1 << pin;
		}

#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)

		else if (pin < 16)		// pin 8 = PB0, ..., 15 = PB7
		{
			io->pinRegister = (unsigned char*)&PINB;
			io->portRegister = (unsigned char*)&PORTB;
			io->ddrRegister = (unsigned char*)&DDRB;
			io->bitmask = 1 << (pin - 8);
		}
		else if (pin < 24)		// pin 16 = PC0, ..., 23 = PC7
		{
			io->pinRegister = (unsigned char*)&PINC;
			io->portRegister = (unsigned char*)&PORTC;
			io->ddrRegister = (unsigned char*)&DDRC;
			io->bitmask = 1 << (pin - 16);
		}
		else if (pin < 32)		// pin 24 = PA7, ..., 31 = PA0
		{
			io->pinRegister = (unsigned char*)&PINA;
			io->portRegister = (unsigned char*)&PORTA;
			io->ddrRegister = (unsigned char*)&DDRA;
			io->bitmask = 1 << (31 - pin);
		}

#else

		else if (pin < 14)		// pin 8 = PB0, ..., 13 = PB5 (PB6 and PB7 reserved for external clock)
		{
			io->pinRegister = (unsigned char*)&PINB;
			io->portRegister = (unsigned char*)&PORTB;
			io->ddrRegister = (unsigned char*)&DDRB;
			io->bitmask = 1 << (pin - 8);
		}
		else if (pin < 21)		// pin 14 = PC0, ..., 19 = PC5 (PC6 is reset, PC7 doesn't exist)
		{
			io->pinRegister = (unsigned char*)&PINC;
			io->portRegister = (unsigned char*)&PORTC;
			io->ddrRegister = (unsigned char*)&DDRC;
			io->bitmask = 1 << (pin - 14);
		}
#endif
	}


	// low-level method for setting the data direction (i.e. input or output) of an pin or set of pins
	// described by an IOStruct pointer.
	inline static void setDataDirection(struct IOStruct* ioPin, unsigned char val)
	{
		if (val)
			*(ioPin->ddrRegister) |= ioPin->bitmask;
		else
			*(ioPin->ddrRegister) &= ~ioPin->bitmask;
	}


	// low-level method for setting the PORT register value of an pin or set of pins
	// described by an IOStruct pointer.  If the pin is an input, this lets you choose between
	// setting it as high-impedance (val = 0) or enabling the internal pull-up (val = 1).  If the pin is an
	// output, this lets you choose between driving low (val = 0) and driving high (val = 1).
	// NOTE: if val is 0xFF (255), this method will toggle the PORT register pin(s).
	inline static void setOutputValue(struct IOStruct* ioPin, unsigned char val)
	{
		if (val == 0xFF)
			*(ioPin->portRegister) ^= ioPin->bitmask;
		else if (val)
			*(ioPin->portRegister) |= ioPin->bitmask;
		else
			*(ioPin->portRegister) &= ~ioPin->bitmask;
	}


	// low-level method for reading the value of the PIN register for an pin or set of pins
	// described by an IOStruct pointer.
	inline static unsigned char getInputValue(struct IOStruct* ioPin)
	{
		return *(ioPin->pinRegister) & ioPin->bitmask;
	}


	// high-level method for setting the specified pin as an output with the specified output state.
	// An outputState value of 0 will cause the pin to drive low; a value of 1 will cause the pin to 
	// drive high.  A value of 0xFF (255) will toggle the output state of the pin (i.e. high -> low and
	// low -> high).
	inline static void setOutput(unsigned char pin, unsigned char outputState)
	{
		struct IOStruct registers;
		getIORegisters(&registers, pin);
		setOutputValue(&registers, outputState);
		setDataDirection(&registers, 1);
	}


	// high-level method for setting the specified pin as an input with the specified input state.
	// An inputState value of 0 will cause the pin to be a high-impedance input; a value of 1 will enable the 
	// pin's internal pull-up resistor, which weakly pulls it to Vcc.  A value of 0xFF (255) will toggle the
	// input state.
	inline static void setInput(unsigned char pin, unsigned char inputState)
	{
		struct IOStruct registers;
		getIORegisters(&registers, pin);
		setDataDirection(&registers, 0);
		setOutputValue(&registers, inputState);
	}


	// high-level method for reading the input value of the specified pin.  If the voltage on the pin is low,
	// this method will return 0.  Otherwise, it will return a non-zero result that depends on the value of
	// the pin.
	inline static unsigned char isInputHigh(unsigned char pin)
	{
		struct IOStruct registers;
		getIORegisters(&registers, pin);
		return getInputValue(&registers);
	}

};

extern "C" {
#endif // __cplusplus

// gets a structure with pointers to the three digital I/O registers associated
// with the specified pin (DDR, PORT, and PIN) along with a bitmask with a
// 1 in the position of the specified pin and 0s everywhere else.
static inline void get_io_registers(struct IOStruct* io, unsigned char pin)
{
	io->pinRegister = 0;
	io->portRegister = 0;
	io->ddrRegister = 0;
	io->bitmask = 0;

	if (pin < 8)			// pin 0 = PD0, ..., 7 = PD7
	{
		io->pinRegister = (unsigned char*)&PIND;
		io->portRegister = (unsigned char*)&PORTD;
		io->ddrRegister = (unsigned char*)&DDRD;
		io->bitmask = 1 << pin;
	}

#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
	else if (pin < 16)		// pin 8 = PB0, ..., 15 = PB7
	{
		io->pinRegister = (unsigned char*)&PINB;
		io->portRegister = (unsigned char*)&PORTB;
		io->ddrRegister = (unsigned char*)&DDRB;
		io->bitmask = 1 << (pin - 8);
	}
	else if (pin < 24)		// pin 16 = PC0, ..., 23 = PC7
	{
		io->pinRegister = (unsigned char*)&PINC;
		io->portRegister = (unsigned char*)&PORTC;
		io->ddrRegister = (unsigned char*)&DDRC;
		io->bitmask = 1 << (pin - 16);
	}
	else if (pin < 32)		// pin 24 = PA7, ..., 31 = PA0
	{
		io->pinRegister = (unsigned char*)&PINA;
		io->portRegister = (unsigned char*)&PORTA;
		io->ddrRegister = (unsigned char*)&DDRA;
		io->bitmask = 1 << (31 - pin);
	}

#else

	else if (pin < 14)		// pin 8 = PB0, ..., 13 = PB5 (PB6 and PB7 reserved for external clock)
	{
		io->pinRegister = (unsigned char*)&PINB;
		io->portRegister = (unsigned char*)&PORTB;
		io->ddrRegister = (unsigned char*)&DDRB;
		io->bitmask = 1 << (pin - 8);
	}
	else if (pin < 21)		// pin 14 = PC0, ..., 19 = PC5 (PC6 is reset, PC7 doesn't exist)
	{
		io->pinRegister = (unsigned char*)&PINC;
		io->portRegister = (unsigned char*)&PORTC;
		io->ddrRegister = (unsigned char*)&DDRC;
		io->bitmask = 1 << (pin - 14);
	}
#endif
}


// low-level method for setting the data direction (i.e. input or output) of an pin or set of pins
// described by an IOStruct pointer.
static inline void set_data_direction(struct IOStruct* ioPin, unsigned char val)
{
	if (val)
		*(ioPin->ddrRegister) |= ioPin->bitmask;
	else
		*(ioPin->ddrRegister) &= ~ioPin->bitmask;
}


// low-level method for setting the PORT register value of an pin or set of pins
// described by an IOStruct pointer.  If the pin is an input, this lets you choose between
// setting it as high-impedance (val = 0) or enabling the internal pull-up (val = 1).  If the pin is an
// output, this lets you choose between driving low (val = 0) and driving high (val = 1).
// NOTE: if val is 0xFF (255), this method will toggle the PORT register pin(s).
static inline void set_digital_output_value(struct IOStruct* ioPin, unsigned char val)
{
	if (val == 0xFF)
		*(ioPin->portRegister) ^= ioPin->bitmask;
	else if (val)
		*(ioPin->portRegister) |= ioPin->bitmask;
	else
		*(ioPin->portRegister) &= ~ioPin->bitmask;
}


// low-level method for reading the value of the PIN register for an pin or set of pins
// described by an IOStruct pointer.
static inline unsigned char get_digital_input_value(struct IOStruct* ioPin)
{
	return *(ioPin->pinRegister) & ioPin->bitmask;
}


// high-level method for setting the specified pin as an output with the specified output state.
// An outputState value of 0 will cause the pin to drive low; a value of 1 will cause the pin to 
// drive high.  A value of 0xFF (255) will toggle the output state of the pin (i.e. high -> low and
// low -> high).
static inline void set_digital_output(unsigned char pin, unsigned char outputState)
{
	struct IOStruct registers;
	get_io_registers(&registers, pin);
	set_digital_output_value(&registers, outputState);
	set_data_direction(&registers, 1);
}


// high-level method for setting the specified pin as an input with the specified input state.
// An inputState value of 0 will cause the pin to be a high-impedance input; a value of 1 will enable the 
// pin's internal pull-up resistor, which weakly pulls it to Vcc.  A value of 0xFF (255) will toggle the
// input state.
static inline void set_digital_input(unsigned char pin, unsigned char inputState)
{
	struct IOStruct registers;
	get_io_registers(&registers, pin);
	set_data_direction(&registers, 0);
	set_digital_output_value(&registers, inputState);
}


// high-level method for reading the input value of the specified pin.  If the voltage on the pin is low,
// this method will return 0.  Otherwise, it will return a non-zero result that depends on the value of
// the pin.
static inline unsigned char is_digital_input_high(unsigned char pin)
{
	struct IOStruct registers;
	get_io_registers(&registers, pin);
	return get_digital_input_value(&registers);
}

#ifdef __cplusplus
}
#endif

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
