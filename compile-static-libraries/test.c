#include "orangutan.h"

int main()
{
  lcd_init();
  buzzer_init();

  play("l8 v10 a gafaeada c+adaeafa <a>a<b>ac+ada c+adaeaf"
       "v12 >d cd<b->d<a>d<g>d <f+>d<g>d<a>d<b->d <d>d<e>d<f+>d<g>d <f+>d<g>d<a>d<b-4");
  
  println("Hello!");

  leds_init();

  while(1)
  {
    left_led(0);
    right_led(1);
    delay_ms(100);
    left_led(1);
    right_led(0);
    delay_ms(100);
  }

  return 0;
}
