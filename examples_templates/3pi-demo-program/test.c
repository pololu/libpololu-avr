/*
 * 3pi-demo-program - demo code for the Pololu 3pi Robot
 * 
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>

// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot"; // \xf7 is a greek
													// pi character
const char demo_name_line1[] PROGMEM = "Demo";
const char demo_name_line2[] PROGMEM = "Program";

const char instructions_line1[] PROGMEM = "Use B to";
const char instructions_line2[] PROGMEM = "select.";
const char instructions_line3[] PROGMEM = "Press B";
const char instructions_line4[] PROGMEM = "-try it!";

const char thank_you_line1[] PROGMEM = " Thank ";
const char thank_you_line2[] PROGMEM = "  you!";

const char main_menu_intro_line1[] PROGMEM = "  Main";
const char main_menu_intro_line2[] PROGMEM = "  Menu";

const char menu_bat_test[] PROGMEM = "Battery";
const char menu_led_test[] PROGMEM = "LEDs";
const char menu_lcd_test[] PROGMEM = "LCD";
const char menu_ir_test[] PROGMEM = "Sensors";
const char menu_motor_test[] PROGMEM = "Motors";
const char menu_music_test[] PROGMEM = "Music";
const char menu_pot_test[] PROGMEM = "Trimpot";
const char menu_time_test[] PROGMEM = "Timer";

const char menu_line2[] PROGMEM = "\x7f" "A \xa5" "B C\x7e";
const char back_line2[] PROGMEM = "\6B";

void bat_test();
void led_test();
void lcd_test();
void ir_test();
void motor_test();
void music_test();
void time_test();
void pot_test();
typedef void (*function)();
const function main_menu_functions[] = { bat_test, led_test, pot_test, ir_test, motor_test, music_test, time_test };
const char *main_menu_options[] = { menu_bat_test, menu_led_test, menu_pot_test, menu_ir_test, menu_motor_test, menu_music_test, menu_time_test };
const char main_menu_length = sizeof(main_menu_options)/sizeof(main_menu_options[0]);

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char thank_you_music[] PROGMEM = ">>c32>g32";
const char beep_button_a[] PROGMEM = "!c32";
const char beep_button_b[] PROGMEM = "!e32";
const char beep_button_c[] PROGMEM = "!g32";
const char timer_tick[] PROGMEM = "!v8>>c32";

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
const char note[] PROGMEM = {
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
const char back_arrow[] PROGMEM = {
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

// Displays the battery voltage.
void bat_test()
{
	int bat = read_battery_millivolts();

	print_long(bat);
	print("mV");

	delay_ms(100);
}

char wait_for_250_ms_or_button_b()
{
	int i;
	for(i=0;i<25;i++)
	{
		delay_ms(10);
		if(button_is_pressed(BUTTON_B))
			return 1;
	}
	return 0;
}

// Blinks the LEDs
void led_test()
{
	play("c32");
	print("Red  ");

	red_led(1);
	if(wait_for_250_ms_or_button_b())
		return;
	red_led(0);
	if(wait_for_250_ms_or_button_b())
		return;

	play(">c32");
	lcd_goto_xy(0,0);
	print("Green");

	green_led(1);
	if(wait_for_250_ms_or_button_b())
		return;
	green_led(0);
	if(wait_for_250_ms_or_button_b())
		return;
}

void ir_test()
{
	unsigned int sensors[5]; // an array to hold sensor values

	if(button_is_pressed(BUTTON_C))
		read_line_sensors(sensors, IR_EMITTERS_OFF);
	else
		read_line_sensors(sensors,IR_EMITTERS_ON);

	unsigned char i;

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

	// Display an indicator of whether IR is on or off
	if(button_is_pressed(BUTTON_C))
		print("IR-");
	else
		print("  C");

	delay_ms(100);
}

int m1_speed = 0;
int m2_speed = 0;

void motor_test()
{
	static char m1_back = 0, m2_back = 0;
	char m1_char, m2_char;

	if(button_is_pressed(BUTTON_A))
	{
		if(m1_speed == 0)
		{
			delay_ms(200);

			// If the button is pressed quickly when the motor is off,
			// reverse direction.
			if(!button_is_pressed(BUTTON_A))
				m1_back = !m1_back;
		}
		
		m1_speed += 10;
	}
	else
		m1_speed -= 20;

	if(button_is_pressed(BUTTON_C))
	{
		if(m2_speed == 0)
		{
			delay_ms(200);

			// If the button is pressed quickly when the motor is off,
			// reverse direction.
			if(!button_is_pressed(BUTTON_C))
				m2_back = !m2_back;
		}

		m2_speed += 10;
	}
	else
		m2_speed -= 20;

	if(m1_speed < 0)
		m1_speed = 0;

	if(m1_speed > 255)
		m1_speed = 255;

	if(m2_speed < 0)
		m2_speed = 0;

	if(m2_speed > 255)
		m2_speed = 255;

	// 255/26 = 9, so this gives values in the range of 0 to 9
	m1_char = bar_graph_characters[m1_speed / 26];
	m2_char = bar_graph_characters[m2_speed / 26];
	print_character(m1_char);
	print_character(m1_back ? 'a' : 'A');
	print_character(m1_char);
	lcd_goto_xy(5,0);
	print_character(m2_char);
	print_character(m2_back ? 'c' : 'C');
	print_character(m2_char);

	set_motors(m1_speed * (m1_back ? -1 : 1), m2_speed * (m2_back ? -1 : 1));
	delay_ms(50);
}

const char fugue[] PROGMEM = 
  "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugue_title[] PROGMEM = "       \7 Fugue in D Minor - by J.S. Bach \7       ";

void music_test()
{
	static char fugue_title_pos = 0;
	static long last_shift = 0;
	char c,i;

	if(get_ms() - last_shift > 250)
	{
		for(i=0;i<8;i++)
		{
			c = pgm_read_byte(fugue_title + fugue_title_pos + i);
			print_character(c);
		}
		last_shift = get_ms();

		fugue_title_pos ++;
		if(fugue_title_pos + 8 >= sizeof(fugue_title))
			fugue_title_pos = 0;
	}

	if(!is_playing())
	{
		play_from_program_space(fugue);
	}

	delay_ms(100);
}

void pot_test()
{
	long start = get_ms();
	char elapsed_ms;
	int value;

	set_analog_mode(MODE_10_BIT);
	print_long(read_trimpot());
	print("   "); // to clear the display

	while((elapsed_ms = get_ms() - start) < 100)
	{
		value = read_trimpot();
		play_frequency(value, 200, 15);
		
		if(value < elapsed_ms*10)
		{
			red_led(0);
			green_led(1);
		}
		else
		{
			red_led(1);
			green_led(0);
		}
	}
}

void time_test()
{
	static long elapsed_time = 0;
	static long last_read = 0;
	static long is_ticking = 0;
	static char a_is_pressed = 0;
	static char c_is_pressed = 0;
	static char last_seconds = 0;

	long current_time = get_ms();
	if(is_ticking)
		elapsed_time += current_time - last_read;

	last_read = current_time;

	if(button_is_pressed(BUTTON_A) && !a_is_pressed)
	{
		// reset
		a_is_pressed = 1;
		is_ticking = 0;
		elapsed_time = 0;
		if(!is_playing()) // only play once
			play_from_program_space(beep_button_a);
	}

	// find the end of the button press without stopping
	if(!button_is_pressed(BUTTON_A))
		a_is_pressed = 0;

	if(button_is_pressed(BUTTON_C) && !c_is_pressed)
	{
		// start/stop
		c_is_pressed = 1;
		is_ticking = !is_ticking;
		play_from_program_space(beep_button_c);
	}

	// find the end of the button press without stopping
	if(!button_is_pressed(BUTTON_C))
		c_is_pressed = 0;

	print_long((elapsed_time/1000/60/10)%10); // tens of minutes
	print_long((elapsed_time/1000/60)%10); // minutes
	print_character(':');
	print_long((elapsed_time/1000)%60/10); // tens of seconds
	char seconds = ((elapsed_time/1000)%60)%10;
	print_long(seconds); // seconds
	print_character('.');
	print_long((elapsed_time/100)%10); // tenths of seconds
	print_long((elapsed_time/10)%10); // hundredths of seconds

	// beep every second
	if(seconds != last_seconds && elapsed_time != 0 && !is_playing())
		play_from_program_space(timer_tick);
	last_seconds = seconds;
}

void print_two_lines_delay_1s(const char *line1, const char *line2)
{
	// Play welcome music and display a message
	clear();
	print_from_program_space(line1);
	lcd_goto_xy(0,1);
	print_from_program_space(line2);
	delay_ms(1000);
}

// waits for a button, plays the appropriate beep, and returns the
// button or buttons that were pressed
char wait_for_button_and_beep()
{
	char button = wait_for_button_press(ANY_BUTTON);
	
	if(button & BUTTON_A)
		play_from_program_space(beep_button_a);
	else if(button & BUTTON_B)
		play_from_program_space(beep_button_b);
	else
		play_from_program_space(beep_button_c);

	wait_for_button_release(button);
	return button;
}

// Initializes the 3pi, displays a welcome message, calibrates, and
// plays the initial music.
void initialize()
{
	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	load_custom_characters(); // load the custom characters
	
	play_from_program_space(welcome);
	print_two_lines_delay_1s(welcome_line1,welcome_line2);
	print_two_lines_delay_1s(demo_name_line1,demo_name_line2);
	print_two_lines_delay_1s(instructions_line1,instructions_line2);

	clear();
	print_from_program_space(instructions_line3);
	lcd_goto_xy(0,1);
	print_from_program_space(instructions_line4);

	while(!(wait_for_button_and_beep() & BUTTON_B));

	play_from_program_space(thank_you_music);

	print_two_lines_delay_1s(thank_you_line1,thank_you_line2);
}

void menu_select()
{
	static int menu_index = 0;

	print_two_lines_delay_1s(main_menu_intro_line1,main_menu_intro_line2);

	while(1)
	{
		clear();
		lcd_goto_xy(0,1);
		print_from_program_space(menu_line2);
		lcd_goto_xy(0,0);
		print_from_program_space(main_menu_options[menu_index]);
		lcd_show_cursor(CURSOR_BLINKING);
		// the cursor will be blinking at the end of the option name
	
		// wait for all buttons to be released, then a press
		while(button_is_pressed(ANY_BUTTON));
		char button = wait_for_button_press(ANY_BUTTON);

		if(button & BUTTON_A)
		{
			play_from_program_space(beep_button_a);
			menu_index --;
		}
		else if(button & BUTTON_B)
		{
			lcd_hide_cursor();
			clear();

			play_from_program_space(beep_button_b);
			wait_for_button_release(button);

			while(!button_is_pressed(BUTTON_B))
			{
				lcd_goto_xy(0,1);
				print_from_program_space(back_line2);
				lcd_goto_xy(0,0);
				main_menu_functions[menu_index]();
			}

			set_motors(0,0);
			stop_playing();
			m1_speed = 0;
			m2_speed = 0;
			red_led(0);
			green_led(0);
			play_from_program_space(beep_button_b);

			return;
		}
		else if(button & BUTTON_C)
		{
			play_from_program_space(beep_button_c);
			menu_index ++;
		}

		if(menu_index < 0)
			menu_index = main_menu_length-1;
		if(menu_index >= main_menu_length)
			menu_index = 0;
	}
}

// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	// set up the 3pi
	initialize();

	// This is the "main loop" - it will run forever.
	while(1)
	{
		menu_select();
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
