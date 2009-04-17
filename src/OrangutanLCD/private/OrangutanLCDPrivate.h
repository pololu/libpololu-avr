/*
  OrangutanLCDPrivate.h - Library for using the LCD on the Orangutan LV-168 or 3pi robot.
  Originally written by Tom Benedict as part of Orangutan-Lib.
  Modified by Ben Schmidel, May 14, 2008.
  Released into the public domain.
*/
#ifndef OrangutanLCDPrivate_h
#define OrangutanLCDPrivate_h


// LCD Pinouts:

// On the Orangutan LV-168 and 3pi robot, the LCD control lines are split between
// ports B and D:

#define LCD_RS_E_DDR		DDRD
#define LCD_RS_E_PORT		PORTD
#define LCD_RW_DDR			DDRB
#define LCD_RW_PORT			PORTB
#define LCD_RW				PORT0
#define LCD_RS				PORT2
#define LCD_E				PORT4

#define LCD_PORTB_MASK		((1 << PORT1) | (1 << PORT4) | (1 << PORT5))
#define LCD_PORTD_MASK		(1 << PORT7)

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

#define LCD_PORTB_DATA(data)	(((data&0x01)<<1) | ((data&0x06)<<3))
#define LCD_PORTD_DATA(data)	((data&0x08)<<4)


// Data port bit where the LCD's busy flag lives

#define LCD_BF				7

// Size of the LCD (each starts at zero, so an 8x2 display would use
// the numbers 7 and 1.)

#define LCD_MAX_X			7
#define LCD_MAX_Y			1
 

// Commands

#define	LCD_CLEAR		0x01
#define	LCD_LINE1		0x02
#define LCD_LINE2		0xC0
#define LCD_SHOW_BLINK	0x0F
#define LCD_SHOW_SOLID	0x0E		
#define LCD_HIDE		0x0C
#define LCD_CURSOR_L	0x10
#define LCD_CURSOR_R	0x14
#define LCD_SHIFT_L		0x18
#define LCD_SHIFT_R		0x1C

#endif
