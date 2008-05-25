/*
  OrangutanLCD.h - Library for using the LCD on the Orangutan LV-168
  Originally written by Tom Benedict as part of Orangutan-Lib.
  Modified by Ben Schmidel, May 14, 2008.
  Released into the public domain.
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

	// initializes the LCD hardware; this function MUST be called before
	// the LCD can be used.
	void init();

	// clears the LCD screen and returns the cursor to position (0, 0)
	void clear();

	// prints a character at the current LCD cursor position
	void print(uint8_t character);

	// sends a string to the LCD.  You can send a string
	// longer than 8 characters, but only eight characters show up.
	// The string is printed from wherever the cursor is, and will
	// not span lines.  (This lets you concatenate print statements.)
	void println(const char *str);
	void printIn(const char *str);	// for compatibility with other libraries

	// prints a signed long.  This prints from wherever the cursor is and will
	// not span lines.  (This lets you concatenate print statements.)  This
	// will only print as many characters as there are digits in the number
	// (plus a minus sign if the number is negative).
	void printLong(int32_t value);

	// prints an unsigned long.  This prints from wherever the cursor is and
	// will not span lines.  (This lets you concatenate print statements.)
	// This will only print as many characters as there are digits in the
	// number.
	void printUnsignedLong(uint32_t value);

	// prints a hex nibble (half of a hex byte) at
	// your current cursor location.
	void printHexNibble(uint8_t nibble);

	// prints a two-byte value (word) in hex at your current
	// cursor location.
	void printHex(uint16_t word);

	// lcd_binary prints a byte in binary starting at your current cursor location.
	void printBinary(uint8_t byte);

	// Go to an (X,Y) location on the LCD.  The top line is Y=0, the 
	// leftmost character is X=0.
	void gotoXY(uint8_t x, uint8_t y);

	// Shows the cursor as either a BLINKING or SOLID block
	// cursorType should be either CURSOR_BLINKING or CURSOR_SOLID
	void showCursor(uint8_t cursorType);

	// Hide the cursor
	void hideCursor();

	// shifts the cursor LEFT or RIGHT the given number of positions.
	// direction should be either LCD_LEFT or LCD_RIGHT
	void moveCursor(uint8_t direction, uint8_t num);

	// shifts the display LEFT or RIGHT the given number of
	// positions, delaying for delay_time milliseconds between each shift.
	// This is what you'd use for a scrolling display.
	// direction should be either LCD_LEFT or LCD_RIGHT
	void scroll(uint8_t direction, uint8_t num, uint16_t delay_time);




  private:

  	// Wait for the busy flag to clear on a 4-bit interface
	// This is necessarily more complicated than the 8-bit interface
	// because E must be strobed twice to get the full eight bits
	// back from the LCD, even though we're only interested in one
	// of them.
	void busyWait();

	// Send four bits out the 4-bit interface.  This assumes the busy flag
	// is clear, that our DDRs are all set, etc.  Basically all it does is
	// line up the bits and shove them out the appropriate I/O lines.
	void sendNibble(uint8_t nibble);

	// Send either data or a command on a 4-bit interface
	void send(uint8_t data, uint8_t rs);

};

#endif

