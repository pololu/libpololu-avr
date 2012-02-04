#include <pololu/orangutan.h>

/*
 * motors2: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example uses the OrangutanMotors and OrangutanLCD libraries to drive
 * motors in response to the position of user trimmer potentiometer
 * and to display the potentiometer position and desired motor speed
 * on the LCD.  It uses the OrangutanAnalog library to measure the
 * trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red and green user LEDs.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()                     // run over and over again
{
  while(1) 
  {
    // note that the following line could also be accomplished with:
    // int pot = analogRead(7);
    int pot = read_trimpot();  // determine the trimpot position
    
    // avoid clearing the LCD to reduce flicker
    lcd_goto_xy(0, 0);
    print("pot=");
    print_long(pot);               // print the trim pot position (0 - 1023)
    print("  ");              // overwrite any left over digits
    
    int motorSpeed = (512 - pot) / 2;
    lcd_goto_xy(0, 1);
    print("spd=");
    print_long(motorSpeed);        // print the resulting motor speed (-255 - 255)
    print("   ");
    set_motors(motorSpeed, motorSpeed);  // set speeds of motors 1 and 2

    // all LEDs off
    red_led(0);
    green_led(0);
    // turn green LED on when motors are spinning forward
    if (motorSpeed > 0)
      green_led(1);
    // turn red LED on when motors are spinning in reverse
    if (motorSpeed < 0)
      red_led(1);
    delay_ms(100);
  }
}
