/*
  digital.h - Library for using the digital I/O lines on the
	Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot.  The code is
	all inline, which lets it compile to very small, fast, efficient
	assembly code if you use constants as your inputs.  For example,
	the line:
	
		set_digital_output(3, HIGH);
	
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

const unsigned char INPUT = 0;
const unsigned char OUTPUT = 1;
const unsigned char LOW = 0;
const unsigned char HIGH = 1;
const unsigned char TOGGLE = 0xFF;
const unsigned char HIGH_IMPEDANCE = 0;
const unsigned char PULL_UP_ENABLED = 1;

// port D pins
const unsigned char	IO_D0 = 0;
const unsigned char IO_D1 = 1;
const unsigned char IO_D2 = 2;
const unsigned char IO_D3 = 3;
const unsigned char IO_D4 = 4;
const unsigned char IO_D5 = 5;
const unsigned char IO_D6 = 6;
const unsigned char IO_D7 = 7;

// port B pins
const unsigned char IO_B0 = 8;
const unsigned char IO_B1 = 9;
const unsigned char IO_B2 = 10;
const unsigned char IO_B3 = 11;
const unsigned char IO_B4 = 12;
const unsigned char IO_B5 = 13;


#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega644P__)

const unsigned char IO_B6 = 14;
const unsigned char IO_B7 = 15;

// port C pins
const unsigned char IO_C0 = 16;
const unsigned char IO_C1 = 17;
const unsigned char IO_C2 = 18;
const unsigned char IO_C3 = 19;
const unsigned char IO_C4 = 20;
const unsigned char IO_C5 = 21;
const unsigned char IO_C6 = 22;
const unsigned char IO_C7 = 23;

// port A pins
const unsigned char IO_A0 = 31;
const unsigned char IO_A1 = 30;
const unsigned char IO_A2 = 29;
const unsigned char IO_A3 = 28;
const unsigned char IO_A4 = 27;
const unsigned char IO_A5 = 26;
const unsigned char IO_A6 = 25;
const unsigned char IO_A7 = 24;

#else

// port C pins
const unsigned char IO_C0 = 14;
const unsigned char IO_C1 = 15;
const unsigned char IO_C2 = 16;
const unsigned char IO_C3 = 17;
const unsigned char IO_C4 = 18;
const unsigned char IO_C5 = 19;
const unsigned char IO_C6 = 20;	// only used if RESET pin is changed to be a digital I/O

#endif


struct IOStruct
{
	unsigned char* pinRegister;
	unsigned char* portRegister;
	unsigned char* ddrRegister;
	unsigned char bitmask;
};


void get_io_registers(struct IOStruct* ioPin, unsigned char pin);
void set_data_direction(struct IOStruct* ioPin, unsigned char val);
void set_digital_output_value(struct IOStruct* ioPin, unsigned char val);
unsigned char get_digital_input_value(struct IOStruct* ioPin);
void set_digital_output(unsigned char pin, unsigned char val);
void set_digital_input(unsigned char pin, unsigned char val);
unsigned char is_digital_input_high(unsigned char pin);

#endif
