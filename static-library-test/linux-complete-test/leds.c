#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

void test_leds()
{
  while(!button_is_pressed(ALL_BUTTONS))
  {
    clear();
    printf("Check\n");
    printf("Red");

    red_led(1);
    play_frequency(440,50,15);
    delay_ms(250);

    if(button_is_pressed(ALL_BUTTONS))
      break;

    red_led(0);
    delay_ms(250);

    if(button_is_pressed(ALL_BUTTONS))
      break;
    
    clear();
    printf("Check\n");
    printf("Green");

    green_led(1);
    play_frequency(880,50,15);
    delay_ms(250);

    if(button_is_pressed(ALL_BUTTONS))
      break;

    green_led(0);
    delay_ms(250);
  }

  while(button_is_pressed(ALL_BUTTONS));
  delay_ms(100);
}
