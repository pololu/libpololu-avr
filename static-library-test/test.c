#include <pololu/3pi.h>

void display_readings(const unsigned int *calibrated_values)
{
  unsigned char i;

  for(i=0;i<5;i++) {
    const char display_characters[10] = {' ','.',',','-','+','o','O','0','@','#'};
    // values from 0 to 9
    char c = display_characters[calibrated_values[i]/101];
    print_character(c);
  }
}

int main()
{
  unsigned int counter;

  pololu_3pi_init(2000);
  
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
  delay_ms(500);

  for(counter=0;counter<80;counter++)
  {
    if(counter < 20 || counter >= 60)
      set_motors(40,-40);
    else
      set_motors(-40,40);

    clear();
    lcd_goto_xy(0,1);
    print("Press B");

    calibrate_line_sensors();

    delay_ms(20);
  }
  set_motors(0,0);

  while(!button_is_pressed(BUTTON_B))
  {
    unsigned int sensors[5] = {1,2,3,4,5};
    unsigned int position = read_line(sensors);

    clear();
    print_long(position);
    lcd_goto_xy(0,1);
    display_readings(sensors);

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
       "L16 O5 ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca <b-acadgb-g egdgcg<b-g"
       "<ag<b-gcf<af dfcf<b-f<af <gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
       "O4 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d e>d>c#>db>d>c#b"
       ">c#agaegfe fO5dc#dfdc#<b c#4"
       );

  while(1)
  {
    unsigned int sensors[5] = {1,2,3,4,5};
    unsigned int position = read_line(sensors);

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
