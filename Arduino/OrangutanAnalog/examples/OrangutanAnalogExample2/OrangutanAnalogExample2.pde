#include <OrangutanLCD.h>
#include <OrangutanAnalog.h>

/*
 * OrangutanAnalogExample2 for the Orangutan LV-168
 *
 * This sketch uses the OrangutanAnalog library to read the voltage output
 * of the trimpot (in millivolts) and to read the Orangutan LV-168's
 * temperature sensor in degrees Farenheit.  These values are printed to 
 * the LCD 10 times per second.  This example is intended for use with the
 * Orangutan LV-168 only.
 *
 * You should see the trimpot voltage change as you turn it, and you can
 * get the temperature reading to slowly increase by holding a finger on the
 * underside of the Orangutan LV-168's PCB near the center of the board.
 * Be careful not to zap the board with electrostatic discharge if you
 * try this!
 *
 * http://www.pololu.com/docs/0J17/5.a
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLCD lcd;
OrangutanAnalog analog;

void setup()                      // run once, when the sketch starts
{
  analog.setMode(MODE_10_BIT);    // 10-bit analog-to-digital conversions
}

void loop()                       // run over and over again
{
  lcd.gotoXY(0,0);                // LCD cursor to home position (upper-left)
  lcd.print(analog.toMillivolts(analog.readTrimpot()));  // trimpot output in mV
  lcd.print(" mV  ");             // added spaces are to overwrite left over chars

  lcd.gotoXY(0, 1);               // LCD cursor to start of the second line
  unsigned int temp = analog.readTemperatureF();  // get temp in tenths of a degree F
  lcd.print(temp/10);             // get the whole number of degrees
  lcd.print('.');                 // print the decimal point
  lcd.print(temp - (temp/10)*10); // print the tenths digit
  lcd.print((char)223);           // print a degree symbol
  lcd.print("F  ");               // added spaces are to overwrite left over chars

  delay(100);                     // wait for 100 ms (otherwise LCD flickers too much)
} 
