#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

void test_delay()
{
  OrangutanBuzzer::playFrequency(440,250,8); // should take exactly 250 ms
  OrangutanTime::delayMilliseconds(200);

  printf("\ndelay1");
  assert(OrangutanBuzzer::isPlaying());

  OrangutanTime::delayMilliseconds(100);
  
  printf("\ndelay2");
  assert(!OrangutanBuzzer::isPlaying());
}
