#include <OrangutanPushbuttons.h>
#include <OrangutanLCD.h>

/*
 * OrangutanPushbuttonAndLCDExample for the Orangutan LV-168
 *
 * This example program is indended for use on the Orangutan LV-168.
 * It uses the OrangutanPushbuttons and OrangutanLCD libraries to 
 * detect user input from the pushbuttons and display feedback on the LCD.
 * Note that the OrangutanPushbuttons library does *NOT* require the
 * OrangutanLCD library to function.
 *
 */

OrangutanPushbuttons buttons;
OrangutanLCD lcd;

void setup()                    // run once, when the sketch starts
{
  lcd.init();
  buttons.init();
}

void loop()                     // run over and over again
{
  lcd.clear();
  lcd.println("Waiting");
  
  // wait for either the top or bottom buttons to be pressed
  // store the value of the pressed button in the variable 'button'
  unsigned char button = buttons.waitForPress(TOP_BUTTON | BOTTOM_BUTTON);
  lcd.clear();
  if (button == TOP_BUTTON)     // display the button that was pressed
    lcd.println("top down");
  else
    lcd.println("bot down");
  buttons.waitForRelease(button);  // wait for that button to be released
  lcd.clear();
  lcd.println("released");      // display that the button was released
  delay(1000);
}
