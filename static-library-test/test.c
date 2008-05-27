#include <orangutan.h>
//#include <stdio.h>

int main()
{
  lcd_init();

  print("Press B");

  wait_for_button(BUTTON_B);

  clear();
  print("Go!");

  leds_init();
  motors_init();
  //line_sensors_init();
  //print("int %d", 5);

  while(1)
  {
    unsigned int sensors[5];
    read_line_sensors(sensors);

    if(sensors[1] > sensors[3])
    {
      left_led(1);
      right_led(0);
      set_motors(0,100);
    }
    else
    {
      right_led(1);
      left_led(0);
      set_motors(100,0);
    }
  }

  return 0;
}
