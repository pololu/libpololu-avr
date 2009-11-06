#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

const char efg[] PROGMEM = "m S e8... f O5 <g";

void test_buzzer()
{
  clear();
  printf("Volume\nchange?");

  play_from_program_space(PSTR("o5 l8 v1 c v2 c v3 c v4 c v5 c v6 c v7 c v8 c v9 v v10 c v11 c v12 c v13 c v14 c v15 c"));
  while(is_playing());

  wait_for_button(ALL_BUTTONS);

  clear();
  printf("Nice\nscale?");

  play_from_program_space(PSTR("T400! o4 l4 ms c"));
  while(is_playing());
  play("ML d 1 6 d 16 r 8");
  while(is_playing());
  play_from_program_space(efg);
  while(is_playing());

  while(is_playing());

  // play a long note but cut it off at 250 ms
  play("o4 a2");
  delay_ms(250);
  stop_playing();
  delay_ms(250);

  play_from_program_space(PSTR("ml t240 o4 b l8t120 >c ")); // these notes should take 250 ms, like the others
  play_mode(PLAY_CHECK); // should just play the first note until we check
  delay_ms(500);

  while(is_playing()) // should terminate correctly
    play_check();

  play_mode(PLAY_AUTOMATIC);
  
  wait_for_button(ALL_BUTTONS);
}
