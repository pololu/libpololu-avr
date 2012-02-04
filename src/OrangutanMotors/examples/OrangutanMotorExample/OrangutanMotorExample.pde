#include <OrangutanLEDs.h>
#include <OrangutanAnalog.h>
#include <OrangutanMotors.h>


/*
 * OrangutanMotorExample for the 3pi robot, Orangutan LV-168, Orangutan SV-xx8,
 *   and Baby Orangutan B
 *
 * This example uses the OrangutanMotors library to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red user LED.
 *
 * http://www.pololu.com/docs/0J17/5.e
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanAnalog analog;
OrangutanLEDs leds;
OrangutanMotors motors;

void setup()               // run once, when the sketch starts
{

}

void loop()                // run over and over again
{
  // note that the following line could also be accomplished with:
  // int pot = analog.read(7);
  int pot = analog.readTrimpot();    // determine the trimpot position
  int motorSpeed = pot/2-256;  // turn pot reading into number between -256 and 255
  if(motorSpeed == -256)
    motorSpeed = -255; // 256 is out of range
  motors.setSpeeds(motorSpeed, motorSpeed);
  
  int ledDelay = motorSpeed;
  if(ledDelay < 0)
    ledDelay = -ledDelay;  // make the delay a non-negative number
  ledDelay = 256-ledDelay; // the delay should be short when the speed is high

  leds.red(HIGH);       // turn red LED on
  delay(ledDelay);

  leds.red(LOW);       // turn red LED off
  delay(ledDelay);
}
