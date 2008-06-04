#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_delay()
{
  play_frequency(440,250,8); // should take exactly 250 ms
  delay_ms(200);

  printf("\ndelay1");
  assert(is_playing());

  delay_ms(100);
  
  printf("\ndelay2");
  assert(!is_playing());

}
