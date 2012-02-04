#include <pololu/orangutan.h>

/*
 * motors1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example uses the OrangutanMotors functions to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red user LED.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  while(1)
  {
    // note that the following line could also be accomplished with:
    // int pot = analogRead(7);
    int pot = read_trimpot();    // determine the trimpot position
    int motorSpeed = pot/2-256;  // turn pot reading into number between -256 and 255
	if(motorSpeed == -256)
		motorSpeed = -255; // 256 is out of range
    set_motors(motorSpeed, motorSpeed);
  
    int ledDelay = motorSpeed;
	if(ledDelay < 0)
	  ledDelay = -ledDelay;  // make the delay a non-negative number
	ledDelay = 256-ledDelay; // the delay should be short when the speed is high

    red_led(1);       // turn red LED on
    delay_ms(ledDelay);

    red_led(0);       // turn red LED off
	delay_ms(ledDelay);
  }
}
