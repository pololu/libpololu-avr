/*
 * wheel-encoders1: example for the Orangutan LV, SV, SVP, or X2.  It
 * will work on the Baby Orangutan, though the LCD functions will have
 * no effect.
 * Note that the Orangutan SVP has two dedicated ports for quadrature
 * encoders, so using the PololuWheelEncoders library is not necessary
 * on the Orangutan SVP (the encoders are handled by an auxiliary MCU).
 *
 * This example measures the outputs of two encoders, one connected to
 * ports PC2 and PC3, and another connected to ports PC4 and PC5.
 * Those pins are not easily accessible on the Orangutan SVP or
 * Orangutan X2, so you will probably want to change the pin
 * assignments if you are using one of those devices.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#include <pololu/orangutan.h>

int main()
{
  // Initialize the encoders and specify the four input pins.
  encoders_init(IO_C2, IO_C3, IO_C4, IO_C5);

  while(1)
  {
    // Read the counts for motor 1 and print to LCD.
    lcd_goto_xy(0,0);
    print_long(encoders_get_counts_m1());
    print(" ");

    // Read the counts for motor 2 and print to LCD.
    lcd_goto_xy(4,0);
    print_long(encoders_get_counts_m2());
    print(" ");

    // Print encoder errors, if there are any.
    if(encoders_check_error_m1())
    {
      lcd_goto_xy(0,1);
      print("Error 1");
    }
    if(encoders_check_error_m2())
    {
      lcd_goto_xy(0,1);
      print("Error 2");
    }

    delay_ms(50);
  }
}
