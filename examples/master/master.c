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

void slave_set_motors()
{
	// set the motor speeds
	char message[4] = {0xC1, speed1, 0xC5, speed2};
	serial_send_blocking(message,4); // m1 forward at that speed
}

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
		if(serial_receive_blocking(buffer, 6, 100))
			continue;

		clear();
		print("Connect");
		lcd_goto_xy(0,1);
		buffer[6] = 0;
		print(buffer);

		// play a tune
		char tune[] = "\xB3 l16o6gab>c";
		tune[1] = sizeof(tune)-3;
		serial_send_blocking(tune,sizeof(tune)-1);

		// wait
		wait_for_button(ALL_BUTTONS);

		// calibrate
		time_reset();
		
		while(get_ms() < 250)
		{
			
		}

		unsigned char speed1 = 0, speed2 = 0;

		// read sensors in a loop
		while(1)
		{
			serial_send("\x86",1);
      
			// read 10 characters
			if(serial_receive_blocking(buffer, 10, 100))
				break;

			// get the battery voltage
			serial_send("\xB1",1);
      
			// read 2 bytes
			int battery_millivolts[1];
			if(serial_receive_blocking((char *)battery_millivolts, 2, 100))
				break;

			// display readings
			display_levels((unsigned int*)buffer);
			lcd_goto_xy(0,1);
			print_long(battery_millivolts[0]);
			print(" mV  ");
      
			delay_ms(10);

			// if button A is pressed, increase motor1 speed
			if(button_is_pressed(BUTTON_A) && speed1 < 127)
				speed1 ++;
			else if(speed1 > 1)
				speed1 -= 2;
			else if(speed1 > 0)
				speed1 = 0;

			// if button C is pressed, control motor2
			if(button_is_pressed(BUTTON_C) && speed2 < 127)
				speed2 ++;
			else if(speed2 > 1)
				speed2 -= 2;
			else if(speed2 > 0)
				speed2 = 0;

			remote_set_motors(speed1, speed2);
		}
	}

	while(1);
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
