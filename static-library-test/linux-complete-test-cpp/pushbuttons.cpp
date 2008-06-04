#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

static OrangutanPushbuttons pushbuttons;

void test_pushbuttons()
{

  OrangutanLCD::clear();
  printf("Press\nA/BOT");

  pushbuttons.waitForPress(BOTTOM_BUTTON);
  unsigned char button = pushbuttons.waitForRelease(BOTTOM_BUTTON);

  printf(" %d",button);
  assert(button == BOTTOM_BUTTON);

  OrangutanLCD::clear();
  printf("Press\nB/MID");
  button = pushbuttons.waitForButton(ALL_BUTTONS);

  printf(" %d",button);
  assert(button == MIDDLE_BUTTON);

  OrangutanLCD::clear();
  printf("Press\nC/TOP");
  button = pushbuttons.waitForButton(TOP_BUTTON);

  printf(" %d",button);
  assert(button == TOP_BUTTON);
}

