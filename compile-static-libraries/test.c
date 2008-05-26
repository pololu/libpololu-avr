#include <stdio.h>
#include "orangutan.h"

int main()
{
  unsigned int sensor_values[5];

  lcd_init();
  buzzer_init();

  play("l8 v10 a gafaeada c+adaeafa <a>a<b>ac+ada c+adaeaf"
       "v12 >d cd<b->d<a>d<g>d <f+>d<g>d<a>d<b->d <d>d<e>d<f+>d<g>d <f+>d<g>d<a>d<b-4");
  
  while(1)
  {
    read_line_sensors(sensor_values);
    printf("\n%d",sensor_values[0]);
    delay_ms(100);
  }

  return 0;
}
