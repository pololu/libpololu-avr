#include <pololu/Orangutan.h>
#include "assert.h"

static OrangutanPushbuttons pushbuttons;
static OrangutanLCD lcd;

void test_pushbuttons()
{

  lcd.clear();
  lcd.print("Press\nA/BOT");

  pushbuttons.waitForPress(BOTTOM_BUTTON);
  unsigned char button = pushbuttons.waitForRelease(BOTTOM_BUTTON);

  lcd.print((int)button);
  assert(button == BOTTOM_BUTTON);

  lcd.clear();
  lcd.print("Press\nB/MID");
  button = pushbuttons.waitForButton(ALL_BUTTONS);

  lcd.print((int)button);
  assert(button == MIDDLE_BUTTON);

  lcd.clear();
  lcd.print("Press\nC/TOP");
  button = pushbuttons.waitForButton(TOP_BUTTON);

  lcd.print((int)button);
  assert(button == TOP_BUTTON);
}

