/*
 * PololuWheelEncoders example for the Orangutan LV/SV-xx8.
 *
 * This example measures the outputs of two encoders, one connected to
 * ports PC2 and PC3, and another connected to ports PC4 and PC5.
 *
 * http://www.pololu.com/docs/0J20?section=6.k
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#include <pololu/orangutan.h>
#include <avr/interrupt.h>

int main()
{
  encoders_init(16,17,18,19); // the Arduino numbers for ports PC2 - PC5

  while(1)
  {
    lcd_goto_xy(0,0);
    print_long(encoders_get_counts_m1());
    print(" ");

    lcd_goto_xy(4,0);
    print_long(encoders_get_counts_m2());
    print(" ");

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
