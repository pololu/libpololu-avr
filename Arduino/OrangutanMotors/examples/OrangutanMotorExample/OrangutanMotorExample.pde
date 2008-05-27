#include <OrangutanMotors.h>

/*
 * OrangutanMotorAndLCDExample for the Orangutan LV-168 or Baby Orangutan B
 *
 * This example program is indended for use on the Orangutan LV-168 or
 * Baby Orangutan B.  It uses the OrangutanMotors library to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.
 *
 */

OrangutanMotors motors(ENABLE_MILLIS);  // argument allows millis() to keep working
unsigned long prevMillis = 0;

void setup()               // run once, when the sketch starts
{
  pinMode(1, OUTPUT);      // sets the red LED pin as an output
  motors.init();
}

void loop()                // run over and over again
{
  int pot = analogRead(7); // measure the trim pot position
  int speed = (512 - pot) / 2;  // turn pot reading into number between -256 and 256
  motors.setM1Speed(speed);
  motors.setM2Speed(speed);
  
  // the following code blinks the LED at a rate that's approximately
  // inversely proportional to the position of the user trim pot
  if (millis() - prevMillis >= pot)
  {
    prevMillis = millis() + 100;
    digitalWrite(1, HIGH); // turn red LED on
  }
  delay(100);
  digitalWrite(1, LOW);    // turn red LED off
}
