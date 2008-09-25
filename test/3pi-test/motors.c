#include <pololu/orangutan.h>
#include "assert.h"

void test_motors()
{
  clear();
  print("Motors");
  lcd_goto_xy(0,1);
  print("Press A");
  wait_for_button(BUTTON_A);

  clear();
  print("L slow");
  lcd_goto_xy(0,1);
  print("Press B");

  set_motors(60,0);
  wait_for_button(BUTTON_B);

  clear();
  print("L fast");
  lcd_goto_xy(0,1);
  print("Press C");

  set_motors(255,0);
  wait_for_button(BUTTON_C);

  clear();
  print("L- slow");
  lcd_goto_xy(0,1);
  print("Press A");

  set_motors(-60,0);
  wait_for_button(BUTTON_A);

  clear();
  print("L- fast");
  lcd_goto_xy(0,1);
  print("Press B");

  set_motors(-255,0);
  wait_for_button(BUTTON_B);

  /// right motor
  clear();
  print("R slow");
  lcd_goto_xy(0,1);
  print("Press C");

  set_motors(0,60);
  wait_for_button(BUTTON_C);

  clear();
  print("R fast");
  lcd_goto_xy(0,1);
  print("Press A");

  set_motors(0,255);
  wait_for_button(BUTTON_A);

  clear();
  print("R- slow");
  lcd_goto_xy(0,1);
  print("Press B");

  set_motors(0,-60);
  wait_for_button(BUTTON_B);

  clear();
  print("R- fast");
  lcd_goto_xy(0,1);
  print("Press C");

  set_motors(0,-255);
  wait_for_button(BUTTON_C);

  set_motors(0,0);
}
