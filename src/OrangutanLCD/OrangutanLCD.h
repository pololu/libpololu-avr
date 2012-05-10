/*
  OrangutanLCD.h - Library for using the LCD on the Orangutan LV, SV, SVP, X2, or 3pi robot.
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
 
#ifndef OrangutanLCD_h
#define OrangutanLCD_h

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "../OrangutanResources/include/OrangutanModel.h"


#define LCD_LEFT			0
#define LCD_RIGHT			1
#define CURSOR_SOLID		0
#define CURSOR_BLINKING		1


#ifdef _ORANGUTAN_SVP

#define LCD_DB4				PORTC2		// PC2
#define LCD_DB5				PORTC3		// PC3
#define LCD_DB6				PORTC4		// PC4
#define LCD_DB7				PORTC5		// PC5

#define LCD_RS_DDR			DDRB
#define LCD_RS_PORT			PORTB
#define LCD_E_DDR			DDRB
#define LCD_E_PORT			PORTB
#define LCD_RW_DDR			DDRB
#define LCD_RW_PORT			PORTB

#define LCD_RS				PORTB0
#define LCD_RW				PORTB1
#define LCD_E				PORTB2

#define LCD_BF_DDR			DDRC
#define LCD_BF_PIN			PINC
#define LCD_BF_PORT			PORTC
#define LCD_BF_MASK			(1 << LCD_DB7)

// Since we're only using four data lines, and since the pins they're
// wired up to don't start with 0, we need to shift them into
// position in order to load their values into the LCD.
//
//	PortC:     7 6 5 4 3 2 1 0
//  LCD Data:      3 2 1 0
//
//  Pass your 4-bit LCD data value to the LCD_PORTC_DATA macro to get
//  the appropriate PORTC value.  You can use the macro as follows:
//
//  PORTC &= ~LCD_PORTC_MASK;
//  PORTC |= LCD_PORTC_DATA(lcdData);

#define LCD_PORTC_MASK			((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7))
#define LCD_PORTC_DATA(data)	((data & 0x0F) << LCD_DB4)


#elif defined(_ORANGUTAN_X2)

#define LCD_RS_DDR			DDRB
#define LCD_RS_PORT			PORTB
#define LCD_E_DDR			DDRB
#define LCD_E_PORT			PORTB
#define LCD_RW_DDR			DDRB
#define LCD_RW_PORT			PORTB

#define LCD_RS				PORTB0
#define LCD_RW				PORTB1
#define LCD_E				PORTB3

#define LCD_BF_DDR			DDRC
#define LCD_BF_PIN			PINC
#define LCD_BF_PORT			PORTC
#define LCD_BF_MASK			(1 << PORTC7)


#else

// On the Orangutan LV-168 and 3pi robot, the LCD control lines are split between
// ports B and D:

#define LCD_DB4				PORTB1		// PB1
#define LCD_DB5				PORTB4		// PB4
#define LCD_DB6				PORTB5		// PB5
#define LCD_DB7				PORTD7		// PD7

#define LCD_RS_DDR			DDRD
#define LCD_RS_PORT			PORTD
#define LCD_E_DDR			DDRD
#define LCD_E_PORT			PORTD
#define LCD_RW_DDR			DDRB
#define LCD_RW_PORT			PORTB

#define LCD_RW				PORTB0
#define LCD_RS				PORTD2
#define LCD_E				PORTD4

#define LCD_BF_DDR			DDRD
#define LCD_BF_PIN			PIND
#define LCD_BF_PORT			PORTD
#define LCD_BF_MASK			(1 << LCD_DB7)

// Since we're only using four data lines, and since the pins they're
// wired up to don't start with 0, we need to shift them into
// position in order to load their values into the LCD.  Port B uses
// bits 1, 4, and 5.  We need to make our data line up like this:
//
//	PortB:     7 6 5 4 3 2 1 0
//  LCD Data:      2 1     0
//
//  PortD:     7 6 5 4 3 2 1 0
//  LCD Data:  3
//
//  Pass your 4-bit LCD data value to the LCD_PORTB_DATA and LCD_PORTD_DATA
//  macros to get the respective PORTB and PORTD values.  You can use them
//  as follows:
//
//  PORTB &= ~LCD_PORTB_MASK;
//  PORTB |= LCD_PORTB_DATA(lcdData);
//  PORTD &= ~LCD_PORTD_MASK;
//  PORTD |= LCD_PORTD_DATA(lcdData);

#define LCD_PORTB_MASK			((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6))
#define LCD_PORTD_MASK			(1 << LCD_DB7)
#define LCD_PORTB_DATA(data)	(((data & 0x01) <<1 ) | ((data & 0x06) << 3))
#define LCD_PORTD_DATA(data)	((data & 0x08) << 4)


#endif

// Commands

#define LCD_CLEAR		0x01
#define LCD_SHOW_BLINK	0x0F
#define LCD_SHOW_SOLID	0x0E		
#define LCD_HIDE		0x0C
#define LCD_CURSOR_L	0x10
#define LCD_CURSOR_R	0x14
#define LCD_SHIFT_L		0x18
#define LCD_SHIFT_R		0x1C

#ifdef __cplusplus

class OrangutanLCD
{
  public:
  	
    // constructor
	OrangutanLCD();
	
	// Send either data or a command
	// If we are using a 4-bit interface, only the low nibble is
	// sent when numSends == 1; otherwise, first the high nibble is sent
	// and then the low nibble is sent.
	// If we are using an 8-bit interface, numSends has no effect: the data is
	// sent via a single 8-bit transfer.
	static void send(unsigned char data, unsigned char rs, unsigned char numSends);
	
	static inline void send_cmd(unsigned char cmd)
	{
		send(cmd, 0, 2);
	}

	static inline void send_4bit_cmd(unsigned char cmd)
	{
		send(cmd, 0, 1);
	}

	static inline void send_data(unsigned char data)
	{
		send(data, 1, 2);
	}

	// clears the LCD screen and returns the cursor to position (0, 0)
	static void clear();

	// prints an ASCII character at the current LCD cursor position
	static void print(char character);
	static inline void print(unsigned char character)
	{
		print((char)character);
	};
	
	// sends a string to the LCD.  You can send a string
	// longer than 8 characters, but only eight characters show up.
	// The string is printed from wherever the cursor is, and will
	// not span lines.  (This lets you concatenate print statements.)
	static void print(const char *str);

	// Sends a PROGMEM string to the LCD.
	static void printFromProgramSpace(const char *str);

#ifndef LIB_ORANGUTAN
	static void printIn(const char *str);  // for compatibility with other libs
#endif
	
	// prints signed and unsigned integer values at the current cursor
	// position and will not span lines.
	static void print(unsigned long value);
	static void print(long value);
	static inline void print(unsigned int value)
	{
		print((unsigned long)value);
	};
	static inline void print(int value)
	{
		print((long)value);
	};

	// prints a two-byte value (word) in hex at your current
	// cursor location.
	static void printHex(unsigned int word);

	// prints a one-byte value in hex at your current location
	static void printHex(unsigned char byte);

	// lcd_binary prints a byte in binary starting at your current cursor location.
	static void printBinary(unsigned char byte);

	// Go to an (X,Y) location on the LCD.  The top line is Y=0, the 
	// leftmost character is X=0.
	static void gotoXY(unsigned char x, unsigned char y);

	// Shows the cursor as either a BLINKING or SOLID block
	// cursorType should be either CURSOR_BLINKING or CURSOR_SOLID
	static void showCursor(unsigned char cursorType);

	// Hide the cursor
	static void hideCursor();

	// shifts the cursor LEFT or RIGHT the given number of positions.
	// direction should be either LCD_LEFT or LCD_RIGHT
	static void moveCursor(unsigned char direction, unsigned char num);

	// shifts the display LEFT or RIGHT the given number of
	// positions, delaying for delay_time milliseconds between each shift.
	// This is what you'd use for a scrolling display.
	// direction should be either LCD_LEFT or LCD_RIGHT
	static void scroll(unsigned char direction, unsigned char num, 
						unsigned int delay_time);

	// Loads a custom character into the character memory of the LCD.
	// The parameter 'number' is a character value between 0 and 7,
	// which represents the character that will be customized.  That is
	// The pointer 'picture_p' is a pointer to an 8 byte array in
	// program space containing the picture data.  This kind of array
	// may be defined as follows:
	//
	// #include <avr/pgmspace.h>
	// const char pi[] PROGMEM = { 0b11100, .... }
	//
	// The first byte represets row one, which has, in this example,
	// three black pixels on the left, followed by two clear
	// pixels.  Subsequent values specify the pixels for the remaining
	// rows of the 5x8 character.
	//
	// After loading all desired custom characters, clear() should be
	// called to reset the LCD.
	static void loadCustomCharacter(const char *picture_p, unsigned char number);

	// Initializes the LCD library for printf support.  After this,
	// printf will start sending characters to the LCD.
	static void initPrintf();	// uses default width and height for device
	static void initPrintf(unsigned char lcdWidth, unsigned char lcdHeight);
	

  private:
  
	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
	
	// initializes the LCD hardware; this function MUST be called before
	// the LCD can be used.  It is called if needed by the inline
	// member function init(), which is called if needed by send().
	static void init2();

  	// Wait for the busy flag to clear on a 4-bit interface
	// This is necessarily more complicated than the 8-bit interface
	// because E must be strobed twice to get the full eight bits
	// back from the LCD, even though we're only interested in one
	// of them.
	static void busyWait();

	// Send data via the 4- or 8-bit interface.  This assumes the busy flag
	// is clear, that our DDRs are all set, etc.  Basically all it does is
	// line up the bits and send them out the appropriate I/O lines while
	// strobing the E control line.
	static void sendData(unsigned char data);
	
	// prints a hex nibble (half of a hex byte) at
	// your current cursor location.
	static void printHexNibble(unsigned char nibble);
};

extern "C" {
#endif // __cplusplus

void lcd_init_printf(void);
void lcd_init_printf_with_dimensions(unsigned char width, unsigned char height);
void clear(void);
void print(const char *str);
void print_from_program_space(const char *str);
void print_character(char c);
void print_long(long value);
void print_unsigned_long(unsigned long value);
void print_binary(unsigned char value);
void print_hex(unsigned int value);
void print_hex_byte(unsigned char value);
void lcd_goto_xy(unsigned char col, unsigned char row);
void lcd_show_cursor(unsigned char cursorType);
void lcd_hide_cursor(void);
void lcd_move_cursor(unsigned char direction, unsigned char num);
void lcd_scroll(unsigned char direction, unsigned char num, 
		unsigned int delay_time);
void lcd_load_custom_character(const char *picture, unsigned char number);

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
