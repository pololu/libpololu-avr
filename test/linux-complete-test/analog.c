#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

#define abs(x) ((x)<0?(-(x)):(x))

void test_analog()
{
  // test that set/get mode works
  set_analog_mode(MODE_8_BIT);
  printf("\nGet8BIT");
  assert(MODE_8_BIT == get_analog_mode());

  set_analog_mode(MODE_10_BIT);
  printf("\nGet10BIT");
  assert(MODE_10_BIT == get_analog_mode());

  // read the trimpot in 10 bit mode and compare it to 8 bit mode
  int x1 = analog_read(7);

  set_analog_mode(MODE_8_BIT);
  delay_ms(1); // required for readings to stabilize

  int x2 = analog_read(7);

  printf("\n8BIT10BIT %d %d",x1,x2);
  assert( abs((x1>>2) - x2) < 10 );

  // make sure that the average reading is more stable than individual readings
  set_analog_mode(MODE_10_BIT);
  unsigned char i;
  int min = 1023, max = 0, avg_min = 1023, avg_max = 0;
  
  for(i=0;i<10;i++)
  {
    int x1 = analog_read(7);
    int x2 = analog_read_average(7,256);

    if(x1 > max) max = x1;
    if(x1 < min) min = x1;

    if(x2 > avg_max) avg_max = x2;
    if(x2 < avg_min) avg_min = x2;

    printf("\nAvgComp %03x %03x", x1, x2);
    assert( abs(x1-x2) < 10);
  }

  printf("\nAB%03x%03x%03x%03x",max,min,avg_max,avg_min);
  assert( max - min >= avg_max - avg_min);

  // check that temp C and F return appropriate values in 10bit mode
  set_analog_mode(MODE_10_BIT);
  x1 = analog_read_average(6,100);

  int expect_temp_f = (((int)(analog_read_average_millivolts(TEMP_SENSOR, 20)) * 12) - 634) / 13;
  int expect_temp_c = (((int)(analog_read_average_millivolts(TEMP_SENSOR, 20) * 20)) - 7982) / 39;
  int temp_f = read_temperature_f();
  int temp_c = read_temperature_c();

  printf("\nTF10 %d %d", expect_temp_f, temp_f);
  assert( expect_temp_f/5 == temp_f/5 );

  printf("\nTC10 %d %d", expect_temp_c, temp_c);
  assert( expect_temp_c/5 == temp_c/5 );

  // try temp in 8bit mode
  set_analog_mode(MODE_8_BIT);
  delay_ms(1); // required for readings to stabilize?
  temp_f = read_temperature_f();
  temp_c = read_temperature_c();

  printf("\nTF8 %d %d", expect_temp_f, temp_f);
  assert( (expect_temp_f - temp_f) <= 20 );

  printf("\nTC8 %d %d", expect_temp_c, temp_c);
  assert( abs(expect_temp_c - temp_c) <= 20 );

  // test background conversion
  set_analog_mode(MODE_10_BIT);
  delay_ms(1); // required for readings to stabilize
  x1 = analog_read_average(6,100);
  
  start_analog_conversion(6);

  while(analog_is_converting())
    printf("\nConvert");
  
  x2 = analog_conversion_result();
  printf("%d %d", x1, x2);
  assert( abs(x1 - x2) < 10 );

  // make sure to_millivolts works in 8 and 10 bit mode
  set_analog_mode(MODE_10_BIT);

  x1 = 5000;
  x2 = to_millivolts(1023);
  printf("\nmV1 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 2498;
  x2 = to_millivolts(511);
  printf("\nmV2 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 0;
  x2 = to_millivolts(0);
  printf("\nmV3 %d %d",x1,x2);
  assert( x1 == x2 );

  set_analog_mode(MODE_8_BIT);

  x1 = 5000;
  x2 = to_millivolts(255);
  printf("\nmV4 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 2490;
  x2 = to_millivolts(127);
  printf("\nmV5 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 0;
  x2 = to_millivolts(0);
  printf("\nmV6 %d %d",x1,x2);
  assert( x1 == x2 );
}
