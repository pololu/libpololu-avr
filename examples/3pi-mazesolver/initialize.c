#include <pololu/3pi.h>

const prog_char bars[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

const prog_char left_bar[] PROGMEM = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000
};

const prog_char right_bar[] PROGMEM = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00000
};

void display_values(unsigned int *values, unsigned int max)
{
  unsigned char i;

  unsigned char characters[] = {' ',0,1,2,3,4,5,255};

  lcd_goto_xy(0,1);
  for(i=0;i<5;i++)
  {
    // get characters[0] to characters[7]
    print_character(characters[values[i]*8/(max+1)]);
  }
}

unsigned char initialize()
{
  unsigned int counter;
  unsigned char button;

  pololu_3pi_init(2500);

  // Load bar graph characters.
  // Together with space and the solid block at 255, this makes almost
  // all possible bar heights, with two to spare.
  unsigned char i;
  for(i=0;i<6;i++)
  {
    lcd_load_custom_character(bars+i,i);
  }
  lcd_load_custom_character(left_bar,6);
  lcd_load_custom_character(right_bar,7);

  while(!button_is_pressed(BUTTON_B))
  {
    int bat = read_battery_millivolts();

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

    calibrate_line_sensors(IR_EMITTERS_ON);

    delay_ms(20);
  }
  set_motors(0,0);

  while(!(button = button_is_pressed(ALL_BUTTONS)))
  {
    unsigned int sensors[5];
    unsigned int position = read_line(sensors, IR_EMITTERS_ON);

    clear();

    // we want pos = 0-499 to print zero spaces, etc
    unsigned char spaces = (position+500)/1000;
    for(i=0;i<spaces;i++)
      print_character(' ');

    // print the bar to the left, middle, or right, as appropriate
    if(position == 0)
      print_character(6); // all the way on the left
    else if(position == 4000)
      print_character(7); // all the way on the right
    else if((position+500)%1000 <= 333)
      print_character(6);
    else if((position+500)%1000 <= 666)
      print_character('|');
    else print_character(7);

    lcd_goto_xy(0,1);
    display_values(sensors,1000);

    delay_ms(50);
  }
  wait_for_button_release(button);

  return button;
}
