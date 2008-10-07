#include <pololu/orangutan.h>

/*
 */

// Data for generating the characters used in load_custom_characters
// and display_readings.  By reading levels[] starting at various
// offsets, we can generate all of the 7 extra characters needed for a
// bargraph.  This is also stored in program space.
const char levels[] PROGMEM = {
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

// This character is a musical note.
const prog_char note[] PROGMEM = {
	0b00100,
	0b00110,
	0b00101,
	0b00101,
	0b00100,
	0b11100,
	0b11100,
	0b00000,
};

// This character is a back arrow.
const prog_char back_arrow[] PROGMEM = {
	0b00000,
	0b00010,
	0b00001,
	0b00101,
	0b01001,
	0b11110,
	0b01000,
	0b00100,
};

// This function loads custom characters into the LCD.  Up to 8
// characters can be loaded; we use them for 6 levels of a bar graph
// plus a back arrow and a musical note character.
void load_custom_characters()
{
	lcd_load_custom_character(levels+0,0); // no offset, e.g. one bar
	lcd_load_custom_character(levels+1,1); // two bars
	lcd_load_custom_character(levels+2,2); // etc...
	lcd_load_custom_character(levels+4,3); // skip level 3
	lcd_load_custom_character(levels+5,4);
	lcd_load_custom_character(levels+6,5);
	lcd_load_custom_character(back_arrow,6);
	lcd_load_custom_character(note,7);
	clear(); // the LCD must be cleared for the characters to take effect
}

// 10 levels of bar graph characters
const char bar_graph_characters[10] = {' ',0,0,1,2,3,3,4,5,255};

void display_levels(unsigned int *sensors)
{
  clear();
  int i;
  for(i=0;i<5;i++) {
    // Initialize the array of characters that we will use for the
    // graph.  Using the space, an extra copy of the one-bar
    // character, and character 255 (a full black box), we get 10
    // characters in the array.

    // The variable c will have values from 0 to 9, since
    // values are in the range of 0 to 2000, and 2000/201 is 9
    // with integer math.
    char c = bar_graph_characters[sensors[i]/201];

    // Display the bar graph characters.
    print_character(c);
  }
}

int main()
{
  char buffer[20];

  // load the bar graph
  load_custom_characters();

  // configure serial clock for 115.2 kbaud
  serial_set_baud_rate(115200);

  // wait for the device to show up
  while(1)
  {
    clear();
    print("Master");

    serial_send("\x81",1);

    // read up to 20 characters
    serial_receive(buffer, 6);

    // wait 100 ms for a response
    time_reset();
    while(!serial_receive_buffer_full() && get_ms() < 100);

    // loop unless we found a slave
    if(!serial_receive_buffer_full())
      continue;

    clear();
    print("Connect");
    lcd_goto_xy(0,1);
    buffer[6] = 0;
    print(buffer);

    // wait
    wait_for_button(ALL_BUTTONS);

    // read sensors in a loop
    while(1)
    {
      serial_send("\x86",1);
      
      // read 10 characters
      serial_receive(buffer, 10);

      // wait 100 ms for a response
      time_reset();
      while(!serial_receive_buffer_full() && get_ms() < 1000);

      // break out of sensor loop if we don't get a response
      if(!serial_receive_buffer_full())
	break;

      // display readings
      display_levels((unsigned int*)buffer);
      
      delay_ms(10);
    }
  }

  while(1);
}
