#include <OrangutanLEDs.h>
#include <OrangutanAnalog.h>
#include <OrangutanMotors.h>


/*
 * OrangutanMotorExample for the 3pi robot, Orangutan LV-168, or Baby Orangutan B
 *
 * This example program is indended for use on the 3pi, Orangutan LV-168, or
 * Baby Orangutan B.  It uses the OrangutanMotors library to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red and green user LEDs.
 *
 * http://www.pololu.com/docs/0J17/5.e
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanAnalog analog;
OrangutanLEDs leds;
OrangutanMotors motors;
unsigned long prevMillis = 0;

void setup()               // run once, when the sketch starts
{

}

void loop()                // run over and over again
{
  // note that the following line could also be accomplished with:
  // int pot = analogRead(7);
  int pot = analog.readTrimpot();    // determine the trimpot position
  int motorSpeed = (512 - pot) / 2;  // turn pot reading into number between -256 and 256
  motors.setSpeeds(motorSpeed, motorSpeed);
  
  // the following code blinks the red user LED at a rate that's approximately
  // inversely proportional to the position of the user trim pot
  if (millis() - prevMillis >= pot)
  {
    prevMillis = millis() + 100;
    leds.red(HIGH);       // turn red LED on
  }
  delay(100);
  leds.red(LOW);          // turn red LED off
}
