/*
  OrangutanLCD.cpp - Library for using the LCD on the Orangutan LV, SV, SVP, or 3pi robot.
  This library incorporates some code originally written by Tom Benedict as part of Orangutan-Lib.
  Released into the public domain.
*/

/*
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
// The Orangutan LV, SV, SVP and 3pi the LCD in 4-bit mode with E,
// R/W, and RS control lines.


#include <avr/io.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU
#include "private/OrangutanLCDPrivate.h"	// contains all of the macros and pin defines
#include "OrangutanLCD.h"
#include "../OrangutanResources/include/OrangutanModel.h"

// *************************************************************************
// *       Functions specifically tailored for the Orangutan               *
// *************************************************************************


// constructor (doesn't do anything)

OrangutanLCD::OrangutanLCD()
{
}

#ifdef LIB_POLOLU

#include "../OrangutanTime/OrangutanTime.h"		// provides access to delay routines
#include <stdio.h>

/* define putchar and getchar functions for the LCD */
char row1[8]={' ',' ',' ',' ',' ',' ',' ',' '}; /* remember what we write for scrolling */
unsigned char row=0; /* the current cursor position */
unsigned char col=0;
extern "C" int lcd_putchar(char c, FILE *f) {
	unsigned char nextline=0; /* should we go to next line after output? */
	unsigned char repos=0; /* should we relocate */
	unsigned char i;

	/* control characters */
	if(c == '\n') {
		nextline = 1;
	} else if(c == 8) { // ^H
		col--;
		if(col==(unsigned char)-1) { row--; col=7; }
		if(row==(unsigned char)-1) { row=0; col=0; }
		repos = 1;
	} else {
		OrangutanLCD::print(c); /* write the character */
		if(row==1) row1[col]=c; /* remember the character */
		col++;

		if(col==8) nextline = 1;
	}

	if(nextline) {
		if(row==1) {
			/******* scroll! *******/
			OrangutanLCD::gotoXY(0,0); /* draw top row */
			for(i=0;i<8;i++) {
				OrangutanLCD::print(row1[i]);
				row1[i]=' ';
			}
			OrangutanLCD::gotoXY(0,1); /* erase bottom row */
			for(i=0;i<8;i++) {
				OrangutanLCD::print(' ');
			}
		}

		col=0;
		row=1;
		repos=1;
	}

	if(repos) {
		OrangutanLCD::gotoXY(col,row);
	}

	return c;
}

extern "C" int void_getchar(FILE *f) {
	return 0;
}

extern "C" void lcd_goto_xy(int col, int row)
{
	OrangutanLCD::gotoXY(col,row);
}

extern "C" void lcd_init_printf()
{
	OrangutanLCD::initPrintf();
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

#else
#include "wiring.h"		// provides access to delay() and delayMicroseconds()
#endif

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

// Initialize the LCD for a 4-bit interface
// this method is automatically called the first time any LCD member
// function is called
void OrangutanLCD::init2()
{
	// Set up the DDR for the LCD control lines
	LCD_RS_DDR |= 1 << LCD_RS;
	LCD_RW_DDR |= 1 << LCD_RW;
	LCD_E_DDR |= 1 << LCD_E;

	// Wait >15ms
	delay(20);

	// Send 0x3 (last four bits ignored)
	send_cmd(0x30);

	// Wait >4.1ms
	delay(6);

	// Send 0x3 (last four bits ignored)
	send_cmd(0x30);

	// Wait >120us
	delay(2);

	// Send 0x3 (last four bits ignored)
	send_cmd(0x30);

	// Wait >120us
	delay(2);

	// Send 0x2 (last four bits ignored)  Sets 4-bit mode
	send_cmd(0x20);

	// Wait >120us
	delay(2);

	// Send 0x28 = 4-bit, 2-line, 5x8 dots per char
	send_cmd(0x28);

	// Busy Flag is now valid, so hard-coded delays are no longer
	// required.

	// Send 0x08 = Display off, cursor off, blinking off
	send_cmd(0x08);

	// Send 0x01 = Clear display
	send_cmd(0x01);

	// Send 0x06 = Set entry mode: cursor shifts right, don't scroll
	send_cmd(0x06);

	// Send 0x0C = Display on, cursor off, blinking off
	send_cmd(0x0C);
}

// Wait for the busy flag to clear on a 4-bit interface
// This is necessarily more complicated than the 8-bit interface
// because E must be strobed twice to get the full eight bits
// back from the LCD, even though we're only interested in one
// of them.
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

	uint32_t time = millis();

	do
	{
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

		// Wait a small bit
		delayMicroseconds(1);

		// Strobe out the 4 bits we don't care about:

		// Bring E high
		LCD_E_PORT |= 1 << LCD_E;

		// Wait at least 120ns (1us is excessive)
		delayMicroseconds(1);

		// Bring E low
		LCD_E_PORT &= ~(1 << LCD_E);
	}
	while ((data & LCD_BF_MASK) && (millis() - time < 10));

	// To reach here our busy flag must be zero, meaning the LCD is free
	// or the 10ms timeout period has elapsed

	// Restore our DDR information
	LCD_BF_DDR = temp_ddr;
}


// Send four bits out the 4-bit interface.  This assumes the busy flag
// is clear, that our DDRs are all set, etc.  Basically all it does is
// line up the bits and shove them out the appropriate I/O lines.
void OrangutanLCD::sendNibble(unsigned char nibble)
{
#ifdef _ORANGUTAN_SVP
	PORTC = (PORTC & ~LCD_PORTC_MASK) | LCD_PORTC_DATA(nibble);
#else
	PORTB = (PORTB & ~LCD_PORTB_MASK) | LCD_PORTB_DATA(nibble);
	PORTD = (PORTD & ~LCD_PORTD_MASK) | LCD_PORTD_DATA(nibble);
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


// Send either data or a command on a 4-bit interface
void OrangutanLCD::send(unsigned char data, unsigned char rs)
{	
	init();  // initialize the LCD if we haven't already

	// Wait until the busy flag clears
	busyWait();

	// Save our DDR and port information
#ifdef _ORANGUTAN_SVP
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

	// Clear RW and RS
	LCD_RS_PORT &= ~(1 << LCD_RS);
	LCD_RW_PORT &= ~(1 << LCD_RW);

	// Set RS according to what this routine was told to do
	LCD_RS_PORT |= (rs << LCD_RS);

	// Set the data pins as outputs
#ifdef _ORANGUTAN_SVP
	DDRC |= LCD_PORTC_MASK;
#else
	DDRB |= LCD_PORTB_MASK;
	DDRD |= LCD_PORTD_MASK;
#endif

	// Send the high 4 bits
	sendNibble(data >> 4);

	// Send the low 4 bits
	sendNibble(data & 0x0F);

	// Restore our DDR and port information
#ifdef _ORANGUTAN_SVP
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

#ifdef LIB_POLOLU
	unsigned char i;

	// clear out the LCD
	for(i=0;i<8;i++)
		row1[i] = ' ';

	col = 0;
	row = 0;
#endif
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

#ifndef LIB_POLOLU
// other LCD libraries have this incorrectly named method, so it is here to
// provide some semblance of compatibility
void OrangutanLCD::printIn(const char *str)
{
	print(str);
}
#endif

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
// y is a zero-based row indicator (0 <= y <= 1)
void OrangutanLCD::gotoXY(unsigned char x, unsigned char y)
{
	// Memory locations for the start of each line
	// The actual memory locations are 0x00, and 0x40, but since
	// D7 needs to be high in order to set a new memory location, we can go
	// ahead and make the seventh bit of our memory location bytes to 1,
	// which makes the numbers 0x80 and 0xC0:

	unsigned char line_mem[] = {0x80, 0xC0};

	// Grab the location in the LCD's memory of the start of line y,
	// and add X to it to get the right character location.
	send_cmd(line_mem[y] + x);

#ifdef LIB_POLOLU
	// Save it for use with printf.
	col = x;
	row = y;
#endif
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

#ifdef LIB_POLOLU
// Initializes the LCD library for printf support.  After this,
// printf will start sending characters to the LCD.
void OrangutanLCD::initPrintf()
{
	fdevopen(lcd_putchar, void_getchar);
}
#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **