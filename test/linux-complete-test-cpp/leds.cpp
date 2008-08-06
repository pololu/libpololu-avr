#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

extern OrangutanPushbuttons buttons;
extern OrangutanLEDs leds;

void test_leds()
{
  while(!buttons.isPressed(ALL_BUTTONS))
  {
    OrangutanLCD::clear();
    printf("Check\n");
    printf("Red");

    leds.red(1);
    OrangutanBuzzer::playFrequency(440,50,15);
    delay_ms(250);

    if(buttons.isPressed(ALL_BUTTONS))
      break;

    leds.red(0);
    delay_ms(250);

    if(buttons.isPressed(ALL_BUTTONS))
      break;
    
    OrangutanLCD::clear();
    printf("Check\n");
    printf("Green");

    leds.green(1);
    OrangutanBuzzer::playFrequency(880,50,15);
    delay_ms(250);

    if(buttons.isPressed(ALL_BUTTONS))
      break;

    leds.green(0);
    delay_ms(250);
  }

  while(buttons.isPressed(ALL_BUTTONS));
  delay_ms(100);
}
