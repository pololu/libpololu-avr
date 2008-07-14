#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

void test_delay()
{
  OrangutanBuzzer::playFrequency(440,250,8); // should take exactly 250 ms
  OrangutanDelay::ms(200);

  printf("\ndelay1");
  assert(OrangutanBuzzer::isPlaying());

  OrangutanDelay::ms(100);
  
  printf("\ndelay2");
  assert(!OrangutanBuzzer::isPlaying());
}
