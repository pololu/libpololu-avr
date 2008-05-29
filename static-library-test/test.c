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
  play("T120 V8 L16" 
       "agafaea dac+adaea fa<aa<bac#a dac#adaea f"
       "O5 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad "
       "O5 L8 MS <b-d<b-d ML e-<ge-<g MS c<ac<a ML d<fd<f O4 MS b-gb-g ML >c#e>c#e"
       "MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
       );

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
