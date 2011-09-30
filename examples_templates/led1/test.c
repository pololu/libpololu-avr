#include <pololu/orangutan.h>

/*
 * led1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This program uses the OrangutanLEDs functions to control the red and green 
 * LEDs on the 3pi robot or Orangutan.  It will also work to control the red
 * LED on the Baby Orangutan B (which lacks a second, green LED). 
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  while(1)
  {
    red_led(1);               // red LED on
    delay_ms(1000);           // waits for a second
    red_led(0);               // red LED off
    delay_ms(1000);           // waits for a second
    green_led(1);             // green LED on (will not work on the Baby Orangutan)
    delay_ms(500);            // waits for 0.5 seconds
    green_led(0);             // green LED off (will not work on the Baby Orangutan)
    delay_ms(500);            // waits for 0.5 seconds
  }
}
 
