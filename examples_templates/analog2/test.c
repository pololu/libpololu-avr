#include <pololu/orangutan.h>

/*
 * analog2: for the Orangutan LV/SV-xx8
 *
 * This example uses the OrangutanAnalog functions to read the voltage
 * output of the trimpot (in millivolts) and to read the Orangutan
 * LV-168's temperature sensor in degrees Farenheit.  These values are
 * printed to the LCD 10 times per second.  This example is intended
 * for use with the Orangutan LV/SV-xx8 only.
 *
 * You should see the trimpot voltage change as you turn it, and you can
 * get the temperature reading to slowly increase by holding a finger on the
 * underside of the Orangutan LV/SV-xx8's PCB near the center of the board.
 * Be careful not to zap the board with electrostatic discharge if you
 * try this!
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  set_analog_mode(MODE_10_BIT); // 10-bit analog-to-digital conversions

  while(1)                      // run over and over again
  {
    lcd_goto_xy(0,0);           // LCD cursor to home position (upper-left)
    print_long(to_millivolts(read_trimpot()));  // trimpot output in mV
    print(" mV  ");             // added spaces are to overwrite left over chars

    lcd_goto_xy(0, 1);          // LCD cursor to start of the second line

    unsigned int temp = read_temperature_f();  // get temp in tenths of a degree F
    print_long(temp/10);             // get the whole number of degrees
    print_character('.');            // print the decimal point
    print_long(temp - (temp/10)*10); // print the tenths digit
    print_character(223);       // print a degree symbol
    print("F  ");               // added spaces are to overwrite left over chars

    delay_ms(100);              // wait for 100 ms (otherwise LCD flickers too much)
  } 
}
