/*
 * PololuWheelEncoders example for the Orangutan LV/SV-xx8.
 *
 * This example measures the outputs of two encoders, one connected to
 * ports PC2 and PC3, and another connected to ports PC4 and PC5.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#include <pololu/orangutan.h>

int main()
{
  encoders_init(16,17,18,19); // the Arduino numbers for ports PC2 - PC5

  while(1)
  {
    clear();
    print_long(encoders_get_counts_a());

    lcd_goto_xy(0,4);
    print_long(encoders_get_counts_b());

    delay_ms(50);
  }
}
