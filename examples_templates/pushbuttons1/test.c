#include <pololu/orangutan.h>

/*
 * OrangutanPushbuttonExample for the Orangutan LV/SV-xx8
 *
 * This example program is intended for use on the Orangutan
 * LV/SV-xx8.  It uses the OrangutanPushbuttons library to detect user
 * input from the pushbuttons, and it uses the OrangutanLCD library to
 * display feedback on the LCD.
 *
 * http://www.pololu.com/docs/0J20/6.h
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  while(1)
  {
    clear();
    print("Waiting");
    
    // wait for either the top or bottom buttons to be pressed
    // store the value of the pressed button in the variable 'button'
    unsigned char button = wait_for_button_press(TOP_BUTTON | BOTTOM_BUTTON);
    clear();
    if (button == TOP_BUTTON)     // display the button that was pressed
      print("top down");
    else
      print("bot down");
    wait_for_button_release(button);  // wait for that button to be released
    clear();
    print("released");      // display that the button was released
    delay_ms(1000);
  }
}
