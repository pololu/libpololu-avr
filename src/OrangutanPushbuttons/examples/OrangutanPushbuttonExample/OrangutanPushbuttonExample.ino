#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>

/*
 * OrangutanPushbuttonExample: for the 3pi robot, Orangutan LV-168,
 *    and Orangutan SV-xx8
 *
 * This example uses the OrangutanPushbuttons library to detect user input
 * from the pushbuttons, and it uses the OrangutanLCD library to display
 * feedback on the LCD.
 *
 * http://www.pololu.com/docs/0J17/5.f
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanPushbuttons buttons;
OrangutanLCD lcd;

void setup()                    // run once, when the sketch starts
{
}

void loop()                     // run over and over again
{
  lcd.clear();
  lcd.print("Waiting");
  
  // wait for either the top or bottom buttons to be pressed
  // store the value of the pressed button in the variable 'button'
  unsigned char button = buttons.waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
  lcd.clear();
  if (button == TOP_BUTTON)     // display the button that was pressed
    lcd.print("top down");
  else
    lcd.print("bot down");
  buttons.waitForRelease(button);  // wait for that button to be released
  lcd.clear();
  lcd.print("released");      // display that the button was released
  delay(1000);
}
