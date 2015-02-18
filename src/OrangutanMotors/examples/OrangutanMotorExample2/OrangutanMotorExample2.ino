#include <OrangutanLEDs.h>
#include <OrangutanAnalog.h>
#include <OrangutanMotors.h>
#include <OrangutanLCD.h>

/*
 * OrangutanMotorExample2 for the 3pi robot, Orangutan LV-168,
 *    and Orangutan SV-xx8.
 *
 * This example uses the OrangutanMotors and OrangutanLCD libraries to drive
 * motors in response to the position of user trimmer potentiometer
 * and to display the potentiometer position and desired motor speed
 * on the LCD.  It uses the OrangutanAnalog library to measure the
 * trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red and green user LEDs.
 *
 * http://www.pololu.com/docs/0J17/5.e
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLCD lcd;
OrangutanMotors motors;
OrangutanAnalog analog;
OrangutanLEDs leds;

void setup()                    // run once, when the sketch starts
{

}

void loop()                     // run over and over again
{
  // note that the following line could also be accomplished with:
  // int pot = analogRead(7);
  int pot = analog.readTrimpot();  // determine the trimpot position
  
  // avoid clearing the LCD to reduce flicker
  lcd.gotoXY(0, 0);
  lcd.print("pot=");
  lcd.print(pot);               // print the trim pot position (0 - 1023)
  lcd.print("  ");              // overwrite any left over digits
  
  int motorSpeed = (512 - pot) / 2;
  lcd.gotoXY(0, 1);
  lcd.print("spd=");
  lcd.print(motorSpeed);        // print the resulting motor speed (-255 - 255)
  lcd.print("   ");
  motors.setSpeeds(motorSpeed, motorSpeed);  // set speeds of motors 1 and 2
  

  // all LEDs off
  leds.red(LOW);
  leds.green(LOW);
  // turn green LED on when motors are spinning forward
  if (motorSpeed > 0)
    leds.green(HIGH);
  // turn red LED on when motors are spinning in reverse
  if (motorSpeed < 0)
    leds.red(HIGH);
  delay(100);
}
