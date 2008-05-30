#include <OrangutanLEDs.h>

/*
 * OrangutanLEDExample for the Orangutan LV-168 or Baby Orangutan B
 *
 * This sketch uses the OrangutanLEDs library to control the red and green
 * LEDs on the Orangutan LV-168.  It will also work to control the red LED
 * on the Baby Orangutan B (which lacks a second, green LED).
 *
 * http://www.pololu.com/docs/0J17/5.d
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLEDs leds;

void setup()                    // run once, when the sketch starts
{
}

void loop()                     // run over and over again
{
  leds.red(HIGH);               // red LED on
  delay(1000);                  // waits for a second
  leds.red(LOW);                // red LED off
  delay(1000);                  // waits for a second
  leds.green(HIGH);             // green LED on (will not work on the Baby Orangutan)
  delay(500);                   // waits for 0.5 seconds
  leds.green(LOW);              // green LED off (will not work on the Baby Orangutan)
  delay(500);                   // waits for 0.5 seconds
} 
