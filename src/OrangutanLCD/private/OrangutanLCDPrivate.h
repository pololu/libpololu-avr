/*
  OrangutanLCDPrivate.h - Library for using the LCD on the Orangutan LV, SV, SVP, or 3pi robot.
  This library incorporates some code originally written by Tom Benedict as part of Orangutan-Lib.
  Released into the public domain.
*/
#ifndef OrangutanLCDPrivate_h
#define OrangutanLCDPrivate_h

#include "../OrangutanResources/include/OrangutanModel.h"

// LCD Pinouts:

// On the Orangutan LV-168 and 3pi robot, the LCD control lines are split between
// ports B and D:

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


#else


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
