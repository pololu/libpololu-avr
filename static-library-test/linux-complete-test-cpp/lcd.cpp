#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

extern OrangutanLCD lcd;

void test_lcd()
{
  lcd.gotoXY(8,0);
  lcd.print((char)255); // print some junk here

  lcd.clear();
  printf("Verify:\n");

  // prints 1234567 on the last line
  lcd.printBinary(0b01000000);
  lcd.gotoXY(2,1);
  printf("%d",2);

  lcd.gotoXY(4,1);
  lcd.print(-56);

  lcd.gotoXY(3,1);
  lcd.print(34);

  lcd.gotoXY(7,1);
  lcd.print('7');

  lcd.gotoXY(8,0);
  lcd.showCursor(CURSOR_BLINKING);

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  lcd.clear();
  printf("No blinking?");
  lcd.showCursor(CURSOR_SOLID);

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  lcd.hideCursor();
}
