/*
  OrangutanLCD.h - Library for using the LCD on the Orangutan LV, SV, SVP, or 3pi robot.
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
 
#ifndef OrangutanLCD_h
#define OrangutanLCD_h


#define LCD_LEFT		0
#define	LCD_RIGHT		1
#define CURSOR_SOLID	0
#define CURSOR_BLINKING	1


class OrangutanLCD
{
  public:
  	
    // constructor
	OrangutanLCD();
	
	// Send either data or a command on a 4-bit interface
	static void send(unsigned char data, unsigned char rs);
	
	static inline void send_cmd(unsigned char cmd)
	{
		send(cmd, 0);
	}

	static inline void send_data(unsigned char data)
	{
		send(data, 1);
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
	// const prog_char pi[] PROGMEM = { 0b11100, .... }
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
	static void initPrintf();
	

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

	// Send four bits out the 4-bit interface.  This assumes the busy flag
	// is clear, that our DDRs are all set, etc.  Basically all it does is
	// line up the bits and shove them out the appropriate I/O lines.
	static void sendNibble(unsigned char nibble);
	
	// prints a hex nibble (half of a hex byte) at
	// your current cursor location.
	static void printHexNibble(unsigned char nibble);
};

#endif


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
