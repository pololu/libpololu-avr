#include <avr/io.h>
#include "OrangutanLineSensors.h"
#include "OrangutanLCD.h"

int main()
{
  OrangutanLineSensors sensors;
  OrangutanLCD lcd;
  unsigned int sensor_values[5];
  int i;

  lcd.init();

  // read 100 times to slow it down
  for(i=0;i<100;i++)
    sensors.read(sensor_values);

  lcd.clear();
  lcd.printLong(sensor_values[1]);
  lcd.gotoXY(0,1);
  lcd.printLong(sensor_values[2]);

  return 0;
}
