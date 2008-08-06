#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

#define abs(x) ((x)<0?(-(x)):(x))

OrangutanAnalog analog;

void test_analog()
{
  // test that set/get mode works
  analog.setMode(MODE_8_BIT);
  printf("\nGet8BIT");
  assert(MODE_8_BIT == OrangutanAnalog::getMode());

  analog.setMode(MODE_10_BIT);
  printf("\nGet10BIT");
  assert(MODE_10_BIT == OrangutanAnalog::getMode());

  // read the trimpot in 10 bit mode and compare it to 8 bit mode
  int x1 = analog.read(7);

  analog.setMode(MODE_8_BIT);
  delay_ms(1); // required for readings to stabilize

  int x2 = OrangutanAnalog::read(7);

  printf("\n8BIT10BIT %d %d",x1,x2);
  assert( abs((x1>>2) - x2) < 10 );

  // make sure that the average reading is more stable than individual readings
  analog.setMode(MODE_10_BIT);
  unsigned char i;
  int min = 1023, max = 0, avg_min = 1023, avg_max = 0;
  
  for(i=0;i<10;i++)
  {
    int x1 = analog.read(7);
    int x2 = analog.readAverage(7,256);

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
  analog.setMode(MODE_10_BIT);
  x1 = analog.readAverage(6,100);

  int expect_temp_f = ((x1 * 18 + 2) >> 2) - 40;
  int expect_temp_c = ((expect_temp_f - 320)*5+4)/9;
  int temp_f = analog.readTemperatureF();
  int temp_c = analog.readTemperatureC();

  printf("\nTF10 %d %d", expect_temp_f, temp_f);
  assert( expect_temp_f == temp_f );

  printf("\nTC10 %d %d", expect_temp_c, temp_c);
  assert( expect_temp_c == temp_c );

  // try temp in 8bit mode
  analog.setMode(MODE_8_BIT);
  OrangutanTime::delayMilliseconds(1); // required for readings to stabilize?
  temp_f = analog.readTemperatureF();
  temp_c = analog.readTemperatureC();

  printf("\nTF8 %d %d", expect_temp_f, temp_f);
  assert( (expect_temp_f - temp_f) <= 20 );

  printf("\nTC8 %d %d", expect_temp_c, temp_c);
  assert( abs(expect_temp_c - temp_c) <= 20 );

  // test background conversion
  analog.setMode(MODE_10_BIT);
  delay_ms(1); // required for readings to stabilize
  x1 = analog.readAverage(6,100);
  
  analog.startConversion(6);

  while(analog.isConverting())
    printf("\nConvert");
  
  x2 = analog.conversionResult();
  printf("%d %d", x1, x2);
  assert( abs(x1 - x2) < 10 );

  // make sure OrangutanAnalog::toMillivolts works in 8 and 10 bit mode
  analog.setMode(MODE_10_BIT);

  x1 = 5000;
  x2 = OrangutanAnalog::toMillivolts(1023);
  printf("\nmV1 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 2498;
  x2 = OrangutanAnalog::toMillivolts(511);
  printf("\nmV2 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 0;
  x2 = OrangutanAnalog::toMillivolts(0);
  printf("\nmV3 %d %d",x1,x2);
  assert( x1 == x2 );

  analog.setMode(MODE_8_BIT);

  x1 = 5000;
  x2 = OrangutanAnalog::toMillivolts(255);
  printf("\nmV4 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 2490;
  x2 = OrangutanAnalog::toMillivolts(127);
  printf("\nmV5 %d %d",x1,x2);
  assert( x1 == x2 );

  x1 = 0;
  x2 = OrangutanAnalog::toMillivolts(0);
  printf("\nmV6 %d %d",x1,x2);
  assert( x1 == x2 );
}
