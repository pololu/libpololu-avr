#include <pololu/orangutan.h>

/*
 * analog1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example uses the OrangutanAnalog functions to read the voltage
 * output of the trimpot in the background while the rest of the main
 * loop executes.  The LED is flashed so that its brightness appears
 * proportional to the trimpot position.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

unsigned int sum;
unsigned int avg;
unsigned char samples;

int main()
{
  set_analog_mode(MODE_8_BIT);    // 8-bit analog-to-digital conversions
  sum = 0;
  samples = 0;
  avg = 0;
  start_analog_conversion(TRIMPOT);  // start initial conversion

  while(1)
  {
    if (!analog_is_converting())     // if conversion is done...
    {
      sum += analog_conversion_result();  // get result
      start_analog_conversion(TRIMPOT);   // start next conversion
      if (++samples == 20)           // if 20 samples have been taken...
      {
        avg = sum / 20;             // compute 20-sample average of ADC result
        samples = 0;
        sum = 0;
      }
    }
    
    // when avg == 0, the red LED is almost totally off.
    // when avg == 255, the red LED is almost totally on.
    // brightness should scale approximately linearly in between.
    red_led(0);                 // red LED off
    delay_us(256 - avg);
    red_led(1);                 // red LED on
    delay_us(avg+1);
  } 
}
