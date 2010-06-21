#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_lcd()
{
  lcd_goto_xy(8,0);
  print_character(255); // print some junk here

  clear();
  printf("Verify:\n");

  // prints 1234567 on the last line
  print_binary(0b01000000);
  lcd_goto_xy(2,1);
  printf("%d",2);

  lcd_goto_xy(4,1);
  print_long(-56);

  lcd_goto_xy(3,1);
  print_long(34);

  lcd_goto_xy(7,1);
  print_character('7');

  lcd_goto_xy(7,0);
  lcd_show_cursor(CURSOR_BLINKING);

  wait_for_button(ALL_BUTTONS);

  clear();
  printf("No blinking?");
  lcd_show_cursor(CURSOR_SOLID);

  wait_for_button(ALL_BUTTONS);

  lcd_hide_cursor();
}
