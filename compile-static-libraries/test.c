#include <stdio.h>
#include <math.h>
#include "orangutan.h"

int main()
{
  lcd_init();
  buzzer_init();

  /*  play("l8 v10 a gafaeada c+adaeafa <a>a<b>ac+ada c+adaeaf"
      "v12 >d cd<b->d<a>d<g>d <f+>d<g>d<a>d<b->d <d>d<e>d<f+>d<g>d <f+>d<g>d<a>d<b-4");*/
  
  printf("Hello, world!");

  delay_ms(500);

  printf("\n3pi=");

  delay_ms(500);

  printf("%.9f",3*M_PI);

  return 0;
}
