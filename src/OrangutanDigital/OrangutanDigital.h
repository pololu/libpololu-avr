/*
  OrangutanDigital.h - Library for using the digital I/O lines on the
	Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  The code is
	all inline, which lets it compile to very small, fast, efficient
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
	but the methods in this library can lead to easier to write (and
	read) programmatic approaches to dealing with digital I/O.
	
	The digital pins on the AVR default to high-impedance inputs after
	a power-up or reset.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
 * Copyright (c) 2009 Pololu Corporation. For more information, see
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

#include <avr/io.h>


struct IOStruct
{
	unsigned char* pinRegister;
	unsigned char* portRegister;
	unsigned char* ddrRegister;
	unsigned char bitmask;
};



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

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)
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
		else if (pin < 20)		// pin 14 = PC0, ..., 19 = PC5 (PC6 is reset, PC7 doesn't exist)
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
	// described by an IOStruct pointer.  If the pin is an input, this lets you choose between
	// setting it as high-impedance (val = 0) or enabling the internal pull-up (val = 1).  If the pin is an
	// output, this lets you choose between driving low (val = 0) and driving high (val = 1).
	// NOTE: if val is 0xFF, this method will toggle the PORT register pin(s).
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

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
