#include <pololu/orangutan.h>

/*
 * analog3: for the latest revision of the Orangutan SV-328.
 *
 * IMPORTANT NOTE: your board must have the identifiers "org01b03" and
 * "0J1499", or this test will not work!!!
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  set_analog_mode(MODE_10_BIT); // 10-bit analog-to-digital conversions

  while(1)
  {
    // read the internal voltage reference
    int ref1100 = analog_read_average(14,256);

    long temp = 0;
    int i;
    
    for(i=0;i<256;i++)
    {
      start_analog_conversion_using_internal_reference(8);
      while(analog_is_converting());
      temp += analog_conversion_result();
    }

    // Temp now holds a 256 times a 10-bit value relative to the 1.1 reference
    // ref1100 holds the value of the 1.1 reference.  The formula for
    // temperature (deg C) is:
    //
    //   T = +25 C + (x*ref/1023/256 - 315 mV) * 60 C / (65 mV)
    //

    clear();
    print_long(temp);
    lcd_goto_xy(0,1);
    
    int mv = temp*ref1100/256/1023*5000/1023;
    print_long(mv);
    
  } 
}
