#include <OrangutanLEDs.h>
#include <OrangutanAnalog.h>

/*
 * OrangutanAnalogExample for the 3pi, Orangutan SV-xx8, 
 *    Orangutan LV-168, or Baby Orangutan B
 *
 * This sketch uses the OrangutanAnalog library to read the voltage output
 * of the trimpot in the background while the rest of the main loop executes.
 * The LED is flashed so that its brightness appears proportional to the
 * trimpot position.
 *
 * http://www.pololu.com/docs/0J17/5.a
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLEDs leds;
OrangutanAnalog analog;

unsigned int sum;
unsigned int avg;
unsigned char samples;

void setup()                      // run once, when the sketch starts
{
  analog.setMode(MODE_8_BIT);    // 8-bit analog-to-digital conversions
  sum = 0;
  samples = 0;
  avg = 0;
  analog.startConversion(TRIMPOT);  // start initial conversion
}

void loop()                       // run over and over again
{
  if (!analog.isConverting())     // if conversion is done...
  {
    sum += analog.conversionResult();  // get result
    analog.startConversion(TRIMPOT);          // and start next conversion
    if (++samples == 20)
    {
      avg = sum / 20;             // compute 20-sample average of ADC result
      samples = 0;
      sum = 0;
    }
  }
    
  // when avg == 0, the red LED is almost totally off
  // when avg == 255, the red LED is almost totally on
  // brightness should scale approximately linearly in between
  leds.red(LOW);                  // red LED off
  delayMicroseconds(256 - avg);
  leds.red(HIGH);                 // red LED on
  delayMicroseconds(avg + 1);
} 
