#include <pololu.h>

int main()
{
  {
    unsigned char pins[5] = {14,15,16,17,18};
    qtr_rc_init(pins,5,1000,19);
    qtr_emitters_off();
  }

  while(!button_is_pressed(BUTTON_B))
  {
    int battery_voltage;

    set_analog_mode(MODE_10_BIT);

    battery_voltage = read_analog_average(6,10)*5000L*3/2/1023;

    clear();
    print_long(battery_voltage);
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
