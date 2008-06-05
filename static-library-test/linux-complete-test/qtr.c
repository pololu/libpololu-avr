#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"

const prog_char bars[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

void test_qtr()
{
  clear();

  // Load bar graph characters.
  // Together with space and the solid block at 255, this makes all
  // possible bar heights, with two to spare.
  int i;
  for(i=0;i<7;i++)
  {
    lcd_load_custom_character(bars+i,i);
  }

  // test RC
  unsigned char pins[5] = {14,15,16,17,18};
  unsigned int values[5];
  unsigned char characters[] = {' ',0,1,2,3,4,5,6,255};

  qtr_rc_init(pins,5,1000,19);

  // raw values
  while(!button_is_pressed(ALL_BUTTONS))
  {
    clear();

    qtr_read(values);

    printf("Raw %4d",values[0]);

    for(i=0;i<5;i++)
    {
      // get characters[0] to characters[8]
      print_character(characters[values[i]*9/1001]);
    }

    qtr_calibrate();

    delay_ms(50);
  }

  wait_for_button(ALL_BUTTONS);

  // calibrated values
  while(!button_is_pressed(ALL_BUTTONS))
  {
    clear();

    qtr_read_calibrated(values);

    printf("Cal %4d",values[0]);

    for(i=0;i<5;i++)
    {
      // get characters[0] to characters[8]
      print_character(characters[values[i]*9/1001]);
    }

    delay_ms(50);
  }

  wait_for_button(ALL_BUTTONS);
}
