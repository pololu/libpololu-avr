#include <pololu/orangutan>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "assert.h"

const char efg[] PROGMEM = "m S e8... f O5 <g";

extern OrangutanLCD lcd;
extern OrangutanBuzzer buzzer;

void test_buzzer()
{
  lcd.clear();
  printf("Volume\nchange?");

  buzzer.play("o5 l8 v1 c v2 c v3 c v4 c v5 c v6 c v7 c v8 c v9 v v10 c v11 c v12 c v13 c v14 c v15 c");
  while(buzzer.isPlaying());

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  lcd.clear();
  printf("Nice\nscale?");

  buzzer.play("o4 l4 ms c");
  while(buzzer.isPlaying());
  buzzer.play("ML d 1 6 d 16 r 8");
  while(buzzer.isPlaying());
  buzzer.playFromProgramSpace(efg);
  while(buzzer.isPlaying());

  while(buzzer.isPlaying());

  // buzzer.play a long note but cut it off at 250 ms
  buzzer.play("o4 a2");
  delay(250);
  buzzer.stopPlaying();
  delay(250);

  buzzer.play("ml t240 o4 b t120 l8 >c "); // these notes should take 250 ms, like the others
  buzzer.playMode(PLAY_CHECK); // should just buzzer.play the first note until we check
  delay(500);

  while(buzzer.isPlaying()) // should terminate correctly
    buzzer.playCheck();

  buzzer.playMode(PLAY_AUTOMATIC);
  
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);
}
