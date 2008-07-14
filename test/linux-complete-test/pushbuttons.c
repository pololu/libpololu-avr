#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_pushbuttons()
{
  clear();
  printf("Press\nA/BOT");

  wait_for_button_press(BOTTOM_BUTTON);
  unsigned char button = wait_for_button_release(BOTTOM_BUTTON);

  printf(" %d",button);
  assert(button == BOTTOM_BUTTON);

  clear();
  printf("Press\nB/MID");
  button = wait_for_button(ALL_BUTTONS);

  printf(" %d",button);
  assert(button == MIDDLE_BUTTON);

  clear();
  printf("Press\nC/TOP");
  button = wait_for_button(TOP_BUTTON);

  printf(" %d",button);
  assert(button == TOP_BUTTON);
}

