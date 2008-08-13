#include <pololu/orangutan.h>
#include "assert.h"

void test_motors()
{
  clear();
  print("Motors");
  wait_for_button(ALL_BUTTONS);

  clear();
  print("L slow");
  set_motors(60,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("L fast");
  set_motors(255,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("L- slow");
  set_motors(-60,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("L- fast");
  set_motors(-255,0);
  wait_for_button(ALL_BUTTONS);

  /// right motor
  clear();
  print("R slow");
  set_motors(0,60);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("R fast");
  set_motors(0,255);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("R- slow");
  set_motors(0,-60);
  wait_for_button(ALL_BUTTONS);

  clear();
  print("R- fast");
  set_motors(0,-255);
  wait_for_button(ALL_BUTTONS);

  set_motors(0,0);
}
