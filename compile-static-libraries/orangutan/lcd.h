/*
  OrangutanLCD.h - Library for using the LCD on the Orangutan LV-168
  Originally written by Tom Benedict as part of Orangutan-Lib.
  Modified by Ben Schmidel, May 14, 2008.
  Released into the public domain.
*/
#ifndef OrangutanLCD_h
#define OrangutanLCD_h

void lcd_init();
void clear();
void println(const char *str);
void print(unsigned char character);
void print_long(long value);

#endif
