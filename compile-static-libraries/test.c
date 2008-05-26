#include <stdio.h>
#include "orangutan.h"

int main()
{
  unsigned int sensor_values[5];

  lcd_init();
  
  while(1)
  {
    read_line_sensors(sensor_values);
    printf("\n%d",sensor_values[0]);
    delay_ms(100);
  }

  return 0;
}
