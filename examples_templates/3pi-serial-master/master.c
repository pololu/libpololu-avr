#include <pololu/orangutan.h>
#include <string.h>

/*
 * 3pi-serial-master - An example serial master program for the Pololu
 * 3pi Robot.  This can run on any board supported by the library;
 * it is intended as an example of how to use the master/slave
 * routines.
 *
 * http://www.pololu.com/docs/0J21
 * http://www.pololu.com/docs/0J20
 * http://www.poolu.com/
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
		// values are in the range of 0 to 1000, and 1000/101 is 9
		// with integer math.
		char c = bar_graph_characters[sensors[i]/101];

		// Display the bar graph characters.
		print_character(c);
	}
}

// set the motor speeds
void slave_set_motors(int speed1, int speed2)
{
	char message[4] = {0xC1, speed1, 0xC5, speed2};
	if(speed1 < 0)
	{
		message[0] = 0xC2; // m1 backward
		message[1] = -speed1;
	}
	if(speed2 < 0)
	{
		message[2] = 0xC6; // m2 backward
		message[3] = -speed2;
	}
	serial_send_blocking(message,4);
}

// do calibration
void slave_calibrate()
{
	serial_send("\xB4",1);
	int tmp_buffer[5];

	// read 10 characters (but we won't use them)
	serial_receive_blocking((char *)tmp_buffer, 10, 100);
}

// reset calibration
void slave_reset_calibration()
{
	serial_send_blocking("\xB5",1);
}

// calibrate (waits for a 1-byte response to indicate completion)
void slave_auto_calibrate()
{
	int tmp_buffer[1];
	serial_send_blocking("\xBA",1);
	serial_receive_blocking((char *)tmp_buffer, 1, 10000);
}

// sets up the pid constants on the 3pi for line following
void slave_set_pid(char max_speed, char p_num, char p_den, char d_num, char d_den)
{
	char string[6] = "\xBB";
	string[1] = max_speed;
	string[2] = p_num;
	string[3] = p_den;
	string[4] = d_num;
	string[5] = d_den;
	serial_send_blocking(string,6);
}

// stops the pid line following
void slave_stop_pid()
{
	serial_send_blocking("\xBC", 1);
}

// clear the slave LCD
void slave_clear()
{
	serial_send_blocking("\xB7",1);
}

// print to the slave LCD
void slave_print(char *string)
{
	serial_send_blocking("\xB8", 1);
	char length = strlen(string);
	serial_send_blocking(&length, 1); // send the string length
	serial_send_blocking(string, length);
}

// go to coordinates x,y on the slave LCD
void slave_lcd_goto_xy(char x, char y)
{
	serial_send_blocking("\xB9",1);
	serial_send_blocking(&x,1);
	serial_send_blocking(&y,1);
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
		delay_ms(100);
		serial_send("\x81",1);

		if(serial_receive_blocking(buffer, 6, 50))
			continue;
		
		clear();
		print("Connect");
		lcd_goto_xy(0,1);
		buffer[6] = 0;
		print(buffer);

		// clear the slave's LCD and display "Connect" and "OK" on two lines
		// Put OK in the center to test x-y positioning
		slave_clear();
		slave_print("Connect");
		slave_lcd_goto_xy(3,1);
		slave_print("OK");

		// play a tune
		char tune[] = "\xB3 l16o6gab>c";
		tune[1] = sizeof(tune)-3;
		serial_send_blocking(tune,sizeof(tune)-1);

		// wait
		wait_for_button(ANY_BUTTON);

		// reset calibration
		slave_reset_calibration();
		
		time_reset();

		slave_auto_calibrate();

		unsigned char speed1 = 0, speed2 = 0;

		// read sensors in a loop
		while(1)
		{
			serial_send("\x87",1); // returns calibrated sensor values
      
			// read 10 characters
			if(serial_receive_blocking(buffer, 10, 100))
				break;

			// get the line position
			serial_send("\xB6", 1);

			int line_position[1];
			if(serial_receive_blocking((char *)line_position, 2, 100))
				break;

			// get the battery voltage
						serial_send("\xB1",1);
      
			// read 2 bytes
			int battery_millivolts[1];
			if(serial_receive_blocking((char *)battery_millivolts, 2, 100))
				break;

			// display readings
			display_levels((unsigned int*)buffer);

			lcd_goto_xy(5,0);
			line_position[0] /= 4; // to get it into the range of 0-1000
			if(line_position[0] == 1000)
				line_position[0] = 999; // to keep it to a maximum of 3 characters 
			print_long(line_position[0]);
			print("   ");

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

			// if button B is pressed, do PID control
			if(button_is_pressed(BUTTON_B))
				slave_set_pid(40, 1, 20, 3, 2);
			else
			{
				slave_stop_pid();
				slave_set_motors(speed1, speed2);
			}
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
