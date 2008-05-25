#include <OrangutanMotors.h>
#include <OrangutanLCD.h>

/*
 * OrangutanMotorAndLCDExample for the Orangutan LV-168
 *
 * This example program is indended for use on the Orangutan LV-168.
 * It uses the OrangutanMotors and OrangutanLCD libraries to drive
 * motors in response to the position of user trimmer potentiometer
 * and to display the potentiometer position and desired motor speed
 * on the LCD.
 *
 */

OrangutanLCD lcd;
OrangutanMotors motors(ENABLE_MILLIS);  // argument allows millis() to keep working

void setup()                    // run once, when the sketch starts
{
  pinMode(1, OUTPUT);           // sets the red LED pin as an output
  pinMode(7, OUTPUT);           // sets the green LED pin as an output
  lcd.init();
  motors.init();
}

void loop()                     // run over and over again
{
  int pot = analogRead(7);      // measure the trim pot position
  
  // avoid clearing the LCD to reduce flicker
  lcd.gotoXY(0, 0);
  lcd.println("pot=");
  lcd.printLong(pot);           // print the trim pot position (0 - 1023)
  lcd.println("  ");            // overwrite any left over digits
  
  int speed = (512 - pot) / 2;
  lcd.gotoXY(0, 1);
  lcd.println("spd=");
  lcd.printLong(speed);         // print the resulting motor speed (-255 - 255)
  lcd.println("   ");
  motors.setM1Speed(speed);
  motors.setM2Speed(speed);
  
  // all LEDs off
  digitalWrite(1, LOW);
  digitalWrite(7, LOW);
  // turn green LED on when motors are spinning forward
  if (speed > 0)
    digitalWrite(7, HIGH);
  // turn red LED on when motors are spinning in reverse
  if (speed < 0)
    digitalWrite(1, HIGH);
  delay(100);
}
