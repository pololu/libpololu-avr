#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_motors()
{
  clear();
  printf("Motors\nready?");
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Left\nslow");
  set_motors(60,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Left\nfast");
  set_motors(255,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Left\nbk slow");
  set_motors(-60,0);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Left\nbk fast");
  set_motors(-255,0);
  wait_for_button(ALL_BUTTONS);

  /// right motor
  clear();
  printf("Right\nslow");
  set_motors(0,60);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Right\nfast");
  set_motors(0,255);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Right\nbk slow");
  set_motors(0,-60);
  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Right\nbk fast");
  set_motors(0,-255);
  wait_for_button(ALL_BUTTONS);

  set_motors(0,0);
}
