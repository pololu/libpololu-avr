/*
  OrangutanLCD.cpp - Library for using the LCD on the Orangutan LV, SV, SVP, X2, or 3pi robot.
*/

/*
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
 *
 * This library incorporates some code originally written by Tom Benedict
 * as part of Orangutan-Lib and released into the public domain.
 */

// Read and write timing:
// 
// Write							Read
// 
// Set RS & R/W					Set RS & R/W
// Wait >= 40ns					Wait >= 40ns
// Set E High					Set E High
// Wait >= 150ns				Wait >= 120ns
// [Data must be valid by now]	[Data is now valid for read]
// Wait >= 80ns					Wait >= 210ns
// Set E Low					Set E Low
// Wait >= 10ns					RS & R/W can change
// [Data, RS, & R/W can change]
//
// In both cases, E cannot be brought high, then low, then high again
// in less than 500ns.
//
// Even though all that timing information is in nanoseconds, we need
// to be concerned about it.  At 20MHz, each AVR instruction executes
// in 50ns.  There are delays in there that must be longer than 50ns,
// so we must make sure we wait an appropriate amount of time.
//
// Initialization:
// 
// 4-Bit							8-Bit
// 
// Wait >= 15ms					Wait >= 15ms
// Send 0x3						Send 0x30
// Wait >= 4.1ms				Wait >= 4.1ms
// Send 0x3						Send 0x30
// Wait >= 100us				Wait >= 100us
// Send 0x3						Send 0x30
// Wait >= 100us				Wait >= 100us
// Send 0x2
// Wait >= 100us						
// Send 0x2 0x8					Send 0x38
// [Busy Flag is now valid]		[Busy Flag is now valid]
// Check BF						Check BF
// Send 0x0 0x8					Send 0x08
// Check BF						Check BF
// Send 0x0 0x1					Send 0x01
// Check BF						Check BF
// Send 0x0 0x6					Send 0x06
// Check BF						Check BF
// Send 0x0 0xC					Send 0x0C
//
// A point to consider:  Prior to a write, the busy flag (BF)
// must be clear.  During the time BF is not valid, apparently
// it stays clear.  So the BF check can apparently operate
// even when BF is not valid.  It simply will not cause any
// delays in execution.  I do not know if this is universally
// true, but it appears to be true for the two LCD used on the
// Orangutan LV-168.
// 
// Another point:  On 4-bit interfaces, reading the busy flag
// is a little tricky.  The same command that returns the busy
// flag also returns the cursor's current memory address.  This 
// requires 8-bits of I/O, even if we're not interested in the 
// cursor's address.  So to check for the busy flag we have to
// do two 4-bit reads, complete with all the timing mentioned
// above, even though we're discarding the contents of the second
// 4-bit read.
// 
// The Orangutan LV, SV, SVP and 3pi use the LCD in 4-bit mode with E,
// R/W, and RS control lines.  The Orangutan X2 uses the LCD in 8-bit
// mode with E, R/W, and RS control lines.


#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU
#include "OrangutanLCD.h"
#include "../OrangutanResources/include/OrangutanModel.h"

// *************************************************************************
// *       Functions specifically tailored for the Orangutan               *
// *************************************************************************


// constructor (doesn't do anything)

OrangutanLCD::OrangutanLCD()
{
}


#ifndef ARDUINO
#include "../OrangutanTime/OrangutanTime.h"
#else
#include <Arduino.h> // provides access to delay() and delayMicroseconds()
#endif


#include <stdio.h>

// LCD_WIDTH is the number of characters in each line of the LCD.
// printf_chars array lets us remember what was written to the bottom rows (for scrolling)
#ifdef _ORANGUTAN_SVP
  #define LCD_WIDTH 	16
  #define LCD_HEIGHT	2
#elif defined(_ORANGUTAN_X2)
  #define LCD_WIDTH 	20
  #define LCD_HEIGHT	4
#else
  #define LCD_WIDTH 	8
  #define LCD_HEIGHT	2
#endif

char *printf_chars = 0;
unsigned char row = 0; /* the current cursor position */
unsigned char col = 0;
unsigned char numLCDRows = LCD_HEIGHT;
unsigned char numLCDCols = LCD_WIDTH;
#define SAVED_CHARS		(numLCDCols*(numLCDRows-1))

// This function is called by printf.
extern "C" int lcd_putchar(char c, FILE *f)
{
	unsigned char nextline=0; /* should we go to next line after output? */
	unsigned char repos=0; /* should we relocate */
	unsigned char i;

	if (printf_chars == 0)
	{
		return c;
	}

	/* control characters */
	if (c == '\n')
	{
		nextline = 1;
	}
	else if (c == 8) { // ^H
		col--;
		if (col == (unsigned char)-1) { row--; col = numLCDCols-1; }
		if (row == (unsigned char)-1) { row = 0; col = 0; }
		repos = 1;
	}
	else
	{
		OrangutanLCD::print(c); /* write the character */
		if (row != 0) printf_chars[(row-1)*numLCDCols + col]=c; /* remember the character */
		col++;

		if (col == numLCDCols) nextline = 1;
	}

	if (nextline)
	{
		if (row == numLCDRows - 1)
		{
			/******* scroll! *******/
            // Note: because of the way we implement scrolling,
            // it is never possible for the user to use the lower
            // right corner of his LCD.
			char j;
			for (j = 0; j < numLCDRows-2; j++)  	// draw top LCD_HEIGHT-2 rows
			{
				OrangutanLCD::gotoXY(0, j);
				for (i = 0; i < numLCDCols; i++)
				{
					OrangutanLCD::print(printf_chars[j*numLCDCols + i]);
					printf_chars[j*numLCDCols + i] = printf_chars[(j+1)*numLCDCols + i];
				}
			}
			if (numLCDRows >= 2)
			{
				OrangutanLCD::gotoXY(0, numLCDRows-2);	// draw row above bottom row
				for (i = 0; i < numLCDCols; i++)
				{
					OrangutanLCD::print(printf_chars[(numLCDRows-2)*numLCDCols + i]);
					printf_chars[(numLCDRows-2)*numLCDCols + i] = ' ';	// clear saved bottom row
				}
			}
			OrangutanLCD::gotoXY(0, numLCDRows-1);		// erase bottom row
			for (i = 0; i < numLCDCols; i++)
			{
				OrangutanLCD::print(' ');
			}
		}
		else
			row++;
		col = 0;
		repos = 1;
	}

	if (repos)
	{
		OrangutanLCD::gotoXY(col, row);
	}

	return c;
}

extern "C" int void_getchar(FILE *f) {
	return 0;
}

extern "C" void lcd_goto_xy(unsigned char col, unsigned char row)
{
	OrangutanLCD::gotoXY(col,row);
}

extern "C" void lcd_init_printf()
{
	OrangutanLCD::initPrintf();
}

extern "C" void lcd_init_printf_with_dimensions(unsigned char width, unsigned char height)
{
	OrangutanLCD::initPrintf(width, height);
}

extern "C" void clear()
{
	OrangutanLCD::clear();
}

extern "C" void print(const char *str)
{
	OrangutanLCD::print(str);
}

extern "C" void print_from_program_space(const char *str)
{
	OrangutanLCD::printFromProgramSpace(str);
}

extern "C" void print_character(char c)
{
	OrangutanLCD::print(c);
}

extern "C" void print_long(long value)
{
	OrangutanLCD::print(value);
}

extern "C" void print_unsigned_long(unsigned long value)
{
	OrangutanLCD::print(value);
}

extern "C" void print_hex(unsigned int value)
{
	OrangutanLCD::printHex(value);
}

extern "C" void print_hex_byte(unsigned char value)
{
	OrangutanLCD::printHex(value);
}

extern "C" void print_binary(unsigned char value)
{
	OrangutanLCD::printBinary(value);
}

extern "C" void lcd_show_cursor(unsigned char cursorType)
{
	OrangutanLCD::showCursor(cursorType);
}

extern "C" void lcd_hide_cursor()
{
	OrangutanLCD::hideCursor();
}

extern "C" void lcd_move_cursor(unsigned char direction, unsigned char num)
{
	OrangutanLCD::moveCursor(direction, num);
}

extern "C" void lcd_scroll(unsigned char direction, unsigned char num, 
					   unsigned int delay_time)
{
	OrangutanLCD::scroll(direction, num, delay_time);
}

extern "C" void lcd_load_custom_character(const char *picture_p, unsigned char number)
{
	OrangutanLCD::loadCustomCharacter(picture_p, number);
}


#define LCD_CGRAM   6

// Loads a custom character
void OrangutanLCD::loadCustomCharacter(const char *picture_p, unsigned char number)
{
  unsigned char i;

  // Each character takes up 8 bytes of the character memory, so we
  // multiply by 8 to get the address.
  number *= 8;

  for(i=0; i<8; i++)
  {
    // set CG RAM address
    send_cmd((1<<LCD_CGRAM) | (number+i));

    // write character data
    send_data(pgm_read_byte(picture_p+i));
  }
}


unsigned long lcdTimeout;	// in microseconds;

// Initialize the LCD for an 8-bit interface if using the Orangutan X2
// else for a 4-bit interface.
// this method is automatically called the first time any LCD member
// function is called.  See page 45 of the Hitachi HD44780U datasheet
// for LCD initialization procedure details.
void OrangutanLCD::init2()
{	
#ifdef _ORANGUTAN_SVP	// BF pin is floating on Orangutan SVP
						//  which causes problems if LCD code is used with LCD removed
	LCD_BF_DDR &= ~LCD_BF_MASK;		// make pull-up pin an input
	LCD_BF_PORT |= LCD_BF_MASK;		// enable pull-up on BF pin
	lcdTimeout = 30000;		// 20ms

#else
	lcdTimeout = 10000;		// 3ms
#endif
	
	// Set up the DDR for the LCD control lines
	LCD_RS_DDR |= 1 << LCD_RS;
	LCD_RW_DDR |= 1 << LCD_RW;
	LCD_E_DDR |= 1 << LCD_E;

	// Wait >15ms
	delay(30);

#ifdef _ORANGUTAN_X2

	send_cmd(0x30);	// function set
	delay(6);	// wait >4.1ms
	send_cmd(0x30);	// function set
	delay(2);	// wait >100us
	send_cmd(0x30);	// function set
	delay(2);	// wait >100us
	send_cmd(0x38);	// 8-bit, 2 line, 5x8 dots char (busy flag is now valid)

#else	// Orangutan SVP, LV, SV, and 3pi robot

	send_4bit_cmd(0x3);	// function set
	delay(6);	// wait >4.1ms
	send_4bit_cmd(0x3);	// function set
	delay(2);	// wait >100us
	send_4bit_cmd(0x3);	// function set
	delay(2);	// wait >100us
	send_4bit_cmd(0x2);	// 4-bit interface
	delay(2);
	send_cmd(0x28);	// 4-bit, 2 line, 5x8 dots char (busy flag is now valid)

#endif

	send_cmd(0x08);	// display off, cursor off, blinking off
	send_cmd(0x01);	// clear display
	send_cmd(0x06);	// set entry mode: cursor shifts right, no scrolling
	send_cmd(0x0C);	// display on, cursor off, blinking off
}


// Wait for the busy flag to clear.  The 4-bit interface is 
// more complicated than the 8-bit interface because E must
// be strobed twice to get the full eight bits back from
// the LCD, even though we're only interested in one of them.
void OrangutanLCD::busyWait()
{
	uint8_t temp_ddr, data;

	// Save our DDR information
	temp_ddr = LCD_BF_DDR;

	// Set up the data DDR for input
	LCD_BF_DDR &= ~LCD_BF_MASK;

	// Set up RS and RW to read the state of the LCD's busy flag
	LCD_RS_PORT &= ~(1 << LCD_RS);
	LCD_RW_PORT |= 1 << LCD_RW;
	
	unsigned int usCounter = 0;

	do
	{
		delayMicroseconds(1);
		
		// Bring E high
		LCD_E_PORT |= 1 << LCD_E;

		// Wait at least 120ns (1us is excessive)
		delayMicroseconds(1);

		// Get the data back from the LCD
		data = LCD_BF_PIN;

		// That excessive delay means our cycle time on E cannot be
		// shorter than 1000ns (500ns being the spec), so no further
		// delays are required

		// Bring E low
		LCD_E_PORT &= ~(1 << LCD_E);
		
		usCounter += 2;

#ifndef _ORANGUTAN_X2

		// Wait a small bit
		delayMicroseconds(1);

		// When using the 4-bit interface, we still need to
		// strobe out the 4 bits we don't care about:

		// Bring E high
		LCD_E_PORT |= 1 << LCD_E;

		// Wait at least 120ns (1us is excessive)
		delayMicroseconds(1);

		// Bring E low
		LCD_E_PORT &= ~(1 << LCD_E);
		
		usCounter += 2;
#endif
	}
	while ((data & LCD_BF_MASK) && (usCounter < lcdTimeout));

	// To reach here our busy flag must be zero, meaning the LCD is free
	// or the 20ms timeout period has elapsed	

#ifdef _ORANGUTAN_SVP

	if (usCounter >= lcdTimeout)	// if we timeout, LCD might be removed
		lcdTimeout = 50;			// make timeout period much shorter
	else							// if we don't timeout
	{
		lcdTimeout = 30000;			// make timeout period long again
	}
	
#endif

	// Restore our DDR information
	LCD_BF_DDR = temp_ddr;
}


// Send data via the 4- or 8-bit interface.  This assumes the busy flag
// is clear, that our DDRs are all set, etc.  Basically all it does is
// line up the bits and send them out the appropriate I/O lines while
// strobing the E control line.
void OrangutanLCD::sendData(unsigned char data)
{
#ifdef _ORANGUTAN_SVP
	PORTC = (PORTC & ~LCD_PORTC_MASK) | LCD_PORTC_DATA(data);
#elif defined(_ORANGUTAN_X2)
	PORTC = data;
#else
	PORTB = (PORTB & ~LCD_PORTB_MASK) | LCD_PORTB_DATA(data);
	PORTD = (PORTD & ~LCD_PORTD_MASK) | LCD_PORTD_DATA(data);
#endif

	// At this point the four data lines are set, so the Enable pin 
	// is strobed to let the LCD latch them.

	// Bring E high
	LCD_E_PORT |= 1 << LCD_E;
	
	// Wait => 450ns (1us is excessive)
	delayMicroseconds(1);

	// Bring E low
	LCD_E_PORT &= ~(1 << LCD_E);

	delayMicroseconds(1);

	// Dropping out of the routine will take at least 10ns, the time
	// given by the datasheet for the LCD controller to read the
	// nibble on the falling edge of E

	// Our nibble has now been sent to the LCD.
}


// Send either data or a command
void OrangutanLCD::send(unsigned char data, unsigned char rs, unsigned char numSends)
{	
	init();  // initialize the LCD if we haven't already

	// Wait until the busy flag clears
	busyWait();

	// Save our DDR and port information
#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
	unsigned char temp_ddrc, temp_portc;
	temp_ddrc = DDRC;
	temp_portc = PORTC;
#else
	unsigned char temp_ddrb, temp_portb, temp_ddrd, temp_portd;
	temp_ddrb = DDRB;
	temp_portb = PORTB;
	temp_ddrd = DDRD;
	temp_portd = PORTD;
#endif

	// Clear RW and set or clear RS based on the rs argument
	LCD_RW_PORT &= ~(1 << LCD_RW);
	if (rs)
		LCD_RS_PORT |= 1 << LCD_RS;
	else
		LCD_RS_PORT &= ~(1 << LCD_RS);

	// Set the data pins as outputs
#ifdef _ORANGUTAN_X2
	DDRC = 0xFF;	// set entire port as outputs
	sendData(data);	// send data via 8-bit interface
#else

  #ifdef _ORANGUTAN_SVP
	DDRC |= LCD_PORTC_MASK;
  #else
	DDRB |= LCD_PORTB_MASK;
	DDRD |= LCD_PORTD_MASK;
  #endif  // ifdef _ORANGUTAN_SVP

	if (numSends != 1)
		sendData(data >> 4);	// send high nibble via 4-bit interface
	sendData(data & 0x0F);	// send low nibble via 4-bit interface

#endif  // ifdef _ORANGUTAN_X2

	// Restore our DDR and port information
	// TODO: Fix this code so it does not interfere with interrupts that are
	// using PORTB/C/D pins as outputs.  Maybe it should only restore the pins
	// that were actually used by the LCD.
#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
	DDRC = temp_ddrc;
	PORTC = temp_portc;
#else
	DDRD = temp_ddrd;
	PORTD = temp_portd;
	DDRB = temp_ddrb;
	PORTB = temp_portb;
#endif
}



// *************************************************************************
// *                 device-independent LCD functions                      *
// *************************************************************************


// clears the LCD screen and returns the cursor to position (0, 0)
void OrangutanLCD::clear()
{
	send_cmd(LCD_CLEAR);

	if (printf_chars == 0)		// if we haven't used printf(), return now
		return;

	unsigned char i;

	// clear out the LCD
	for(i = 0; i < SAVED_CHARS; i++)
		printf_chars[i] = ' ';

	col = 0;
	row = 0;
}


// prints a single character at the current cursor location
void OrangutanLCD::print(char character)
{
	send_data(character);
}


// sends a string to the LCD.  The string is printed from 
// wherever the cursor is and will not span lines.  (This lets you 
// concatenate print statements.)
void OrangutanLCD::print(const char *str)
{
	while (*str != 0)
		send_data(*str++);
}

// sends a string to the LCD.  The string is printed from 
// wherever the cursor is and will not span lines.  (This lets you 
// concatenate print statements.)
void OrangutanLCD::printFromProgramSpace(const char *str)
{
	char c;
	while ((c = pgm_read_byte(str)) != 0)
	{
		send_data(c);
		str ++;
	}
}

// other LCD libraries have this incorrectly named method, so it is here to
// provide some semblance of compatibility
void OrangutanLCD::printIn(const char *str)
{
	print(str);
}

// prints a signed long.  This prints from wherever the cursor is and will not
// span lines.  (This lets you concatenate print statements.)  This function
// will only print as many characters as there are digits in the number (plus
// a minus sign if the number is negative).
void OrangutanLCD::print(long value)
{
	if (value < 0)
	{
		value = -value;
		send_data('-');		// print the minus sign
	}
	print((unsigned long)value);
}


// prints an unsigned long.  This prints from wherever the cursor is and will
// not span lines.  (This lets you concatenate print statements.)  This
// will only print as many characters as there are digits in the number.
void OrangutanLCD::print(unsigned long value)
{
	unsigned char str[10];
	unsigned char i = 10;

    // Changed digit from a long to char 2009-11-20.  This fixed a bug with
    // print_long on the Orangutan SVP, but I don't know why.  -David
	unsigned char digit;

	do
	{
		digit = value;
		value /= 10;
		digit -= value * 10;
		str[--i] = '0' + (unsigned char)digit;
	}
	while (value != 0);

	for(; i < 10; i++)
		send_data(str[i]);
}


// display a hex nibble (half of a hex byte) at your current cursor location
void OrangutanLCD::printHexNibble(unsigned char nibble)
{
	if (nibble < 10)
		send_data('0' + nibble);
	else
		send_data('A' + (nibble - 10));
}

// display a two-byte value in hex (0000 - FFFF) at the current cursor location
void OrangutanLCD::printHex(unsigned int word)
{
	printHex((unsigned char)(word >> 8));
	printHex((unsigned char)word);
}

// display a single-byte value in hex (00 - FF) at the current cursor location
void OrangutanLCD::printHex(unsigned char byte)
{
	printHexNibble(byte >> 4);	 // display high byte high nibble
	printHexNibble(byte & 0x0F); // display high byte low nibble
}


// display a byte in binary starting at your current cursor location
void OrangutanLCD::printBinary(unsigned char byte)
{
	unsigned char i, bitmask;

	bitmask = 1 << 7;
	for (i = 0; i < 8; i++)
	{
		if (byte & bitmask)
			send_data('1');
		else
			send_data('0');
		bitmask >>= 1;
	}
}


// moves the cursor to the specified (x, y) position
// x is a zero-based column indicator (0 <= x <= 7)
// y is a zero-based row indicator (0 <= y <= LCD rows-1)
void OrangutanLCD::gotoXY(unsigned char x, unsigned char y)
{
	// Memory locations for the start of each line
	// The actual memory locations are 0x00, and 0x40, but since
	// D7 needs to be high in order to set a new memory location, we can go
	// ahead and make the seventh bit of our memory location bytes to 1,
	// which makes the numbers 0x80 and 0xC0:

	unsigned char line_mem[] = {0x80, 0xC0, 0x94, 0xD4};

	// Grab the location in the LCD's memory of the start of line y,
	// and add X to it to get the right character location.
	send_cmd(line_mem[y] + x);

	// Save it for use with printf.
	col = x;
	row = y;
}


// Shows the cursor as either a BLINKING or SOLID block
// cursorType should be either CURSOR_BLINKING or CURSOR_SOLID
void OrangutanLCD::showCursor(unsigned char cursorType)
{
	if (cursorType == CURSOR_BLINKING)
		send_cmd(LCD_SHOW_BLINK);
	else
		send_cmd(LCD_SHOW_SOLID);
}


// Hides the cursor
void OrangutanLCD::hideCursor()
{
	send_cmd(LCD_HIDE);
}


// shifts the cursor LEFT or RIGHT the given number of positions.
// direction should be either LCD_LEFT or LCD_RIGHT
void OrangutanLCD::moveCursor(unsigned char direction, 
							   unsigned char num)
{
	while(num-- > 0)
	{
		if (direction == LCD_LEFT)
			send_cmd(LCD_CURSOR_L);
		else
			send_cmd(LCD_CURSOR_R);
	}
}


// shifts the display LEFT or RIGHT the given number of
// positions, delaying for delay_time milliseconds between each shift.
// This is what you'd use for a scrolling display.
// direction should be either LCD_LEFT or LCD_RIGHT
void OrangutanLCD::scroll(unsigned char direction, unsigned char num, 
						   unsigned int delay_time)
{
	while(num--)
	{
		if (direction == LCD_LEFT)
			send_cmd(LCD_SHIFT_L);
		else
			send_cmd(LCD_SHIFT_R);
		delay(delay_time);
	}
}

// Initializes the LCD library for printf support.  After this,
// printf will start sending characters to the LCD.
void OrangutanLCD::initPrintf()
{
	if (printf_chars != 0)
		free(printf_chars);
		
	printf_chars = (char*)malloc(sizeof(char)*SAVED_CHARS);
	if (printf_chars == 0)
		return;
	unsigned char i;
	for (i = 0; i < SAVED_CHARS; i++)
		printf_chars[i] = ' ';
	fdevopen(lcd_putchar, void_getchar);
}

void OrangutanLCD::initPrintf(unsigned char lcdWidth, unsigned char lcdHeight)
{
	numLCDCols = lcdWidth;
	numLCDRows = lcdHeight;
	initPrintf();
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
