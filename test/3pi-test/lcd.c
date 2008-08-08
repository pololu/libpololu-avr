#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_lcd()
{
  clear();
  print("\xff\xff\xff\xff\xff\xff\xff\xff");
  lcd_goto_xy(0,1);
  print("Top row?");

  wait_for_button(BUTTON_B);

  clear();
  print("Bottom?");
  lcd_goto_xy(0,1);
  print("\xff\xff\xff\xff\xff\xff\xff\xff");

  wait_for_button(BUTTON_B);
}
