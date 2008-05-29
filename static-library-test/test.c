#include <pololu.h>

int main()
{
  pololu_3pi_init(1000);
  
  while(!button_is_pressed(BUTTON_B))
  {
    int bat = battery_millivolts();

    clear();
    print_long(bat);
    print("mV");
    lcd_goto_xy(0,1);
    print("Press B");

    delay_ms(50);
  }

  wait_for_button_release(BUTTON_B);

  clear();
  print("Go!");

  delay_ms(500);

  //  play("T240 L8 a gafaeada c+adaeafa <aa<bac#ada c#adaeaf4");

  while(1)
  {
    unsigned int sensors[5] = {1,2,3,4,5};
    read_line_sensors(sensors);

    clear();
    print_long(sensors[1]);
    lcd_goto_xy(4,0);
    print_long(sensors[2]);
    lcd_goto_xy(0,1);
    print_long(sensors[3]);
    lcd_goto_xy(4,1);
    print_long(sensors[4]);

    delay_ms(50);
    /*
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
    */
  }

  return 0;
}
