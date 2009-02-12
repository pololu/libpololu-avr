#include <pololu/orangutan.h>

/*
 * motors1: for the Orangutan LV-168 or Baby Orangutan B
 *
 * This example program is indended for use on the Orangutan LV-168 or
 * Baby Orangutan B.  It uses the OrangutanMotors functions to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red and green user LEDs.
 *
 * http://www.pololu.com/docs/0J20/6.g
 * http://www.pololu.com
 * http://forum.pololu.com
 */

unsigned long prevMillis = 0;

int main()
{
  int last_proportional=0;

  red_led(1);
  delay_ms(500);
  red_led(0);
  delay_ms(500);
  red_led(1);
  delay_ms(500);
  red_led(0);
  delay_ms(500);

  int up = 327;
  int integral = 0;

  long loopcount = 0;

  while(1)
  {
    set_analog_mode(MODE_10_BIT);
    int proportional = - (int)analog_read_average(5,200) + up; // determine the proportional reading

    integral += proportional;
    if(integral > 300)
      integral = 300;
    if(integral < -300)
      integral = -300;

    int derivative = proportional - last_proportional;
    last_proportional = proportional;

    int motorSpeed;
    motorSpeed = 4*proportional + 12 * derivative + integral + 20;

    if(proportional > 100)
      motorSpeed = 0;

    if(motorSpeed < -255)
      motorSpeed = -255; // 256 is out of range
    if(motorSpeed > 255)
      motorSpeed = 255; // 256 is out of range
    set_motors(motorSpeed, motorSpeed);

    loopcount ++;

    if(proportional > 0)
      red_led(1);
    else
      red_led(0);

  }
}
