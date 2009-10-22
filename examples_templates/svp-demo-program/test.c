/*
 * SVP_demo_program - demo code for the Pololu Orangutan SVP
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

// The Orangutan include file must be at the beginning of any program that
// uses the Pololu AVR library on an Orangutan.
#include <pololu/orangutan.h>

// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.  The word
// PROGMEM tells the compiler to store data in program space.
#include <avr/pgmspace.h>

const char lcd_width = 16;

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char thank_you_music[] PROGMEM = ">>c32>g32";
const char beep_button_top[] PROGMEM = "!c32";
const char beep_button_middle[] PROGMEM = "!e32";
const char beep_button_bottom[] PROGMEM = "!g32";
const char timer_tick[] PROGMEM = "!v8>>c32";

// This function loads custom characters into the LCD.  Up to 8
// characters can be loaded; we use them for 6 levels of a bar graph
// plus a back arrow and a musical note character.
void load_custom_characters()
{
	// Data for generating the characters used in load_custom_characters
	// and display_readings.  By reading levels[] starting at various
	// offsets, we can generate all of the 7 extra characters needed for a
	// bargraph.  This is also stored in program space.
	static const char levels[] PROGMEM = {
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
	static const prog_char note[] PROGMEM = {
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
	static const prog_char back_arrow[] PROGMEM = {
		0b00000,
		0b00010,
		0b00001,
		0b00101,
		0b01001,
		0b11110,
		0b01000,
		0b00100,
	};

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

// 15 levels of bar graph characters.
/**
  Character, Bar Height, 0-7 (high 3 bits of reading)
  ' '        0           0
  0          1           1
  1          2           2
  2          3           3
  3          5           4
  4          6           5
  5          7           6
  255        8           7
**/

unsigned char wait_for_ms_or_middle_button(unsigned char ms)
{
	unsigned long stop_time = get_ms() + ms;
	while(1)
	{
		if(button_is_pressed(MIDDLE_BUTTON))
		{
			return 1;
		}

		if (get_ms() >= stop_time)
		{
			return 0;
		}
	}
}

unsigned char wait_for_250_ms_or_middle_button()
{
	return wait_for_ms_or_middle_button(250);
}

void print_bar(unsigned char three_bits)
{
	const char bar_graph_characters[8] = {' ',0,1,2,3,4,5,255};
	print_character(bar_graph_characters[three_bits]);
}

static void print_analog_reading(unsigned char channel)
{
	// Do a dummy reading and throw it away, so that the real reading
	// is not biased by the previous reading.
	if (!button_is_pressed(BOTTOM_BUTTON))
	{
		analog_read(channel);
	}

	print_bar(analog_read(channel) >> 5);
}

static void print_digital_reading(unsigned char pin)
{
	if (is_digital_input_high(pin))
	{
		print_character('1');
	}
	else
	{
		print_character('0');
	}
}

void analog_test()
{
	static const char analog_labels[] PROGMEM = "\6m 76543210TABCD";

	svp_set_mode(SVP_MODE_ANALOG);
	set_analog_mode(MODE_8_BIT);

	// Turn off the pull-ups by default.
	unsigned char pullups = 0;
	PORTA = 0;

	while(1)
	{
		green_led(1);
		lcd_goto_xy(0,0);

		if (pullups)
		{
			print("\xa5T ");
		}
		else
		{
			print("\xa5t ");
		}

		print_analog_reading(7);
		print_analog_reading(6);
		print_analog_reading(5);
		print_analog_reading(4);
		print_analog_reading(3);
		print_analog_reading(2);
		print_analog_reading(1);
		print_analog_reading(0);
		print_analog_reading(TRIMPOT);
		print_analog_reading(CHANNEL_A);
		print_analog_reading(CHANNEL_B);
		print_analog_reading(CHANNEL_C);
		print_analog_reading(CHANNEL_D);
		green_led(0);

		lcd_goto_xy(0,1);
		print_from_program_space(analog_labels);

		if (button_is_pressed(TOP_BUTTON))
		{
			pullups = !pullups;

			play_from_program_space(beep_button_top);

			if (pullups)
			{
				PORTA = 0xFF;
			}
			else
			{
				PORTA = 0;
			}
			wait_for_button_release(TOP_BUTTON);
		}

		if(wait_for_ms_or_middle_button(40))
		{
			return;
		}
	}
}

// Displays the battery voltage.
void bat_test()
{
	unsigned int bat = read_battery_millivolts_svp();

	print_long(bat);
	print("mV");

	wait_for_250_ms_or_middle_button();
}

void digital_test()
{
	set_digital_input(IO_B3, PULL_UP_ENABLED);
	set_digital_input(IO_C0, PULL_UP_ENABLED);
	set_digital_input(IO_C1, PULL_UP_ENABLED);
	set_digital_input(IO_D0, PULL_UP_ENABLED);
	set_digital_input(IO_D1, PULL_UP_ENABLED);
	set_digital_input(IO_D2, PULL_UP_ENABLED);
	set_digital_input(IO_D3, PULL_UP_ENABLED);

	lcd_goto_xy(0,0);
	print_from_program_space(PSTR("B3=x  C0=x  C1=x"));
	lcd_goto_xy(0,1);
	print_from_program_space(PSTR("\6mid  PD0:3=xxxx"));

	while(1)
	{
		lcd_goto_xy(3,0);
		print_digital_reading(IO_B3);

		lcd_goto_xy(9,0);
		print_digital_reading(IO_C0);

		lcd_goto_xy(15,0);
		print_digital_reading(IO_C1);

		lcd_goto_xy(12,1);
		print_digital_reading(IO_D0);
		print_digital_reading(IO_D1);
		print_digital_reading(IO_D2);
		print_digital_reading(IO_D3);

		if(wait_for_ms_or_middle_button(40))
		{
			return;
		}		
	}

}

// Blinks the LEDs
void led_test()
{
	play("c32");
	print("Red  ");

	red_led(1);
	if(wait_for_250_ms_or_middle_button())
		return;
	red_led(0);
	if(wait_for_250_ms_or_middle_button())
		return;

	play(">c32");
	lcd_goto_xy(0,0);
	print("Green");

	green_led(1);
	if(wait_for_250_ms_or_middle_button())
		return;
	green_led(0);
	if(wait_for_250_ms_or_middle_button())
		return;
}

int m1_speed = 0;
int m2_speed = 0;

void motor_test()
{
	static char m1_back = 0, m2_back = 0;

	if(button_is_pressed(TOP_BUTTON))
	{
		if(m2_speed == 0)
		{
			delay_ms(200);

			// If the button is pressed quickly when the motor is off,
			// reverse direction.
			if(!button_is_pressed(TOP_BUTTON))
				m2_back = !m2_back;
		}
		
		m2_speed += 10;
	}
	else
		m2_speed -= 20;

	if(button_is_pressed(BOTTOM_BUTTON))
	{
		if(m1_speed == 0)
		{
			delay_ms(200);

			// If the button is pressed quickly when the motor is off,
			// reverse direction.
			if(!button_is_pressed(BOTTOM_BUTTON))
				m1_back = !m1_back;
		}

		m1_speed += 10;
	}
	else
		m1_speed -= 20;

	if(m1_speed < 0)
		m1_speed = 0;

	if(m1_speed > 255)
		m1_speed = 255;

	if(m2_speed < 0)
		m2_speed = 0;

	if(m2_speed > 255)
		m2_speed = 255;

    // Shifting to the right by 5 gets the high 3 bits, which gives us a bar graph character.

	print_bar(m2_speed >> 5);
	print_from_program_space(m2_back ? PSTR("top") : PSTR("TOP"));
	print_bar(m2_speed >> 5);
	lcd_goto_xy(5,0);
	print_bar(m1_speed >> 5);
	print_from_program_space(m1_back ? PSTR("bot") : PSTR("BOT"));
	print_bar(m1_speed >> 5);

	set_motors(m1_speed * (m1_back ? -1 : 1), m2_speed * (m2_back ? -1 : 1));
	delay_ms(50);
}

// Octave 7 C# is really loud!

const char music[] PROGMEM = "O6 T40 L16 d#<b<f#<d#<f#<bd#f#"
  "T80 c#<b-<f#<c#<f#<b-c#8"
  "T180 d#b<f#d#f#>bd#f#c#b-<f#c#f#>b-c#8 c>c#<c#>c#<b>c#<c#>c#c>c#<c#>c#<b>c#<c#>c#"
  "c>c#<c#>c#<b->c#<c#>c#c>c#<c#>c#<b->c#<c#>c#"
  "c>c#<c#>c#f>c#<c#>c#c>c#<c#>c#f>c#<c#>c#"
  "c>c#<c#>c#f#>c#<c#>c#c>c#<c#>c#f#>c#<c#>c#d#bb-bd#bf#d#c#b-ab-c#b-f#d#";

const char music_title[] PROGMEM = "               \7 Hungarian Rhapsody No. 2 - by Franz Liszt \7               ";

void music_test()
{
	static unsigned char title_pos = 0;
	unsigned char i;

	for(i = 0; i < lcd_width; i++)
	{
		print_character(pgm_read_byte(music_title + title_pos + i));
	}

	title_pos++;
	if(title_pos + lcd_width >= sizeof(music_title))
	{
		title_pos = 0;
	}

	if (!is_playing())
	{
		play_from_program_space(music);
	}

	wait_for_250_ms_or_middle_button();
}

void pot_test()
{
	static const char trimpot_line1[] PROGMEM = "Adjust trimpot.";

	set_analog_mode(MODE_10_BIT);
	print_from_program_space(trimpot_line1);
	lcd_goto_xy(8,1);
	print_long(read_trimpot());
	print("   "); // to clear the display

	unsigned long start = get_ms();
	unsigned char elapsed_ms;

	while((elapsed_ms = get_ms() - start) < 128)
	{
		unsigned int value = read_trimpot();
		play_frequency(value, 200, 10);
		
		if((elapsed_ms & 0xF) < (value>>6))
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

// Digit should be a number between 0 and 9.
static void print_digit(unsigned char digit)
{
	print_character('0'+digit);
}

void time_test()
{
	static unsigned long elapsed_time = 0;
	static unsigned long last_read;
	static unsigned char is_ticking = 0;
	static unsigned char top_is_pressed = 0;
	static unsigned char bottom_is_pressed = 0;
	static unsigned char last_seconds = 0;

	unsigned long current_time = get_ms();
	if(is_ticking)
	{
		elapsed_time += current_time - last_read;
	}
	last_read = current_time;

	if(button_is_pressed(TOP_BUTTON))
	{
		if(!top_is_pressed)
		{
			// reset
			top_is_pressed = 1;
			is_ticking = 0;
			elapsed_time = 0;
			play_from_program_space(beep_button_top);

			wait_for_button_release(TOP_BUTTON);clear();// tmphax to make this work on org06a01
		}
	}
	else
	{
		top_is_pressed = 0;
	}

	if(button_is_pressed(BOTTOM_BUTTON))
	{
		if(!bottom_is_pressed)
		{
			// start/stop
			bottom_is_pressed = 1;
			is_ticking = !is_ticking;
			play_from_program_space(beep_button_bottom);
		}
	}
	else
	{
		bottom_is_pressed = 0;
	}

	lcd_goto_xy(0,0);
	print_digit(elapsed_time/1000/60/60/10%10); // tens of hours
	print_digit(elapsed_time/1000/60/60%10);    // hours
	print_character(':');
	print_digit(elapsed_time/1000/60/10%10);    // tens of minutes
	print_digit(elapsed_time/1000/60%10);       // minutes
	print_character(':');
	print_digit(elapsed_time/1000%60/10);       // tens of seconds
	unsigned char seconds = elapsed_time/1000%60%10;
	print_digit(seconds);
	print_character('.');
	print_digit(elapsed_time/100%10);           // tenths of seconds
	print_digit(elapsed_time/10%10);            // hundredths of seconds

	// beep every second
	if(seconds != last_seconds && elapsed_time != 0 && !is_playing())
	{
		play_from_program_space(timer_tick);
	}
	last_seconds = seconds;
}

void print_two_lines_delay_1s(const char *line1, const char *line2)
{
	clear();
	if (line1)
	{
		print_from_program_space(line1);
	}
	if (line2)
	{
		lcd_goto_xy(0,1);
		print_from_program_space(line2);
	}
	delay_ms(1000);
}


char receive_buffer[64];

void usb_test()
{
	serial_receive(USB_COMM, receive_buffer, sizeof(receive_buffer));

	char screen_string[lcd_width+1] = "Type in to COM.";
	unsigned char screen_string_length = 0;

	unsigned char update_lcd_soon = 1;

	unsigned long next_update_time = 0;

	while(1)
	{
		while(1)
		{
			// Ask the auxiliary processor if new bytes have been received.
			serial_check();

			unsigned char received_bytes = serial_get_received_bytes(USB_COMM);

			if (received_bytes == 0)
			{
				break;
			}

			// Cancel receiving now so that no more bytes get added to the receive_buffer
		    // while we handle the bytes thare are in there now.
			serial_cancel_receive(USB_COMM);

			unsigned char i;
			for (i = 0; i < received_bytes; i++)
			{
				// Process the new byte that has been received.
				serial_send_blocking(USB_COMM, &receive_buffer[i], 1);

				if (screen_string_length == lcd_width)
				{
					screen_string_length = 0;
				}
				screen_string[screen_string_length] = receive_buffer[i];
				screen_string_length++;
				screen_string[screen_string_length] = 0;
			}

			update_lcd_soon = 1;

			serial_receive(USB_COMM, receive_buffer, sizeof(receive_buffer));
		}

		if (get_ms() > next_update_time)
		{
			update_lcd_soon = 1;
		}

		if (update_lcd_soon)
		{
			if (!usb_power_present())
			{
				lcd_goto_xy(0,0);
				print_from_program_space(PSTR("Connect USB."));
			}
			else if (usb_suspend())
			{
				lcd_goto_xy(0,0);
				print_from_program_space(PSTR("ZZZZZZzzzzzz...."));
			}
			else if(!usb_configured())
			{
				lcd_goto_xy(0,0);
				print_from_program_space(PSTR("Install drivers."));
			}
			else
			{
				lcd_goto_xy(0,0);
				print(screen_string);
				print_from_program_space(PSTR("                "));
			}

			update_lcd_soon = 0;
			next_update_time = get_ms() + 100;
		}

		if (dtr_enabled())
		{
			red_led(1);
		}
		else
		{
			red_led(0);
		}

		if (rts_enabled())
		{
			green_led(1);
		}
		else
		{
			green_led(0);
		}

		if(button_is_pressed(MIDDLE_BUTTON))
		{
			return;
		}
	}

}

// waits for a button, plays the appropriate beep, and returns the
// button or buttons that were pressed
unsigned char wait_for_button_and_beep()
{
	unsigned char button = wait_for_button_press(ALL_BUTTONS);
	
	if(button & TOP_BUTTON)
	{
		play_from_program_space(beep_button_top);
	}
	else if(button & MIDDLE_BUTTON)
	{
		play_from_program_space(beep_button_middle);
	}
	else
	{
		play_from_program_space(beep_button_bottom);
	}

	wait_for_button_release(button);
	return button;
}

// Initializes, displays a welcome message, calibrates, and
// plays the initial music.
void initialize()
{
	load_custom_characters(); // load the custom characters
	
	play_from_program_space(welcome);

	print_two_lines_delay_1s(PSTR("     Pololu     "),
	                         PSTR(" Orangutan SVP  "));

	print_two_lines_delay_1s(PSTR("Demo Program    "), 0);

	print_two_lines_delay_1s(PSTR("Use mid button  "),
							 PSTR("to select.      "));

	clear();
	print_from_program_space(PSTR("Press middle    "));
	lcd_goto_xy(0,1);
	print_from_program_space(PSTR("button: Try it! "));

	while(!(wait_for_button_and_beep() & MIDDLE_BUTTON));

	play_from_program_space(thank_you_music);

	print_two_lines_delay_1s(0, PSTR("   Thank you!   "));
}

void menu_select()
{
	static const char menu_analog_test[] PROGMEM   = "Analog Inputs";
	static const char menu_bat_test[] PROGMEM      = "Battery Voltage";
	static const char menu_digital_test[] PROGMEM  = "Digital Inputs";
	static const char menu_led_test[] PROGMEM      = "LEDs";
	static const char menu_motor_test[] PROGMEM    = "Motors";
	static const char menu_music_test[] PROGMEM    = "Music";
	static const char menu_pot_test[] PROGMEM      = "Trimpot";
	static const char menu_time_test[] PROGMEM     = "Timer";
	static const char menu_usb_test[] PROGMEM      = "USB";

	typedef void (*function)();
	static const function main_menu_functions[] = { analog_test, bat_test, digital_test, led_test, motor_test, music_test, pot_test, time_test, usb_test };
	static const char *main_menu_options[] = { menu_analog_test, menu_bat_test, menu_digital_test, menu_led_test, menu_motor_test, menu_music_test, menu_pot_test, menu_time_test, menu_usb_test };
	static const char main_menu_length = sizeof(main_menu_options)/sizeof(main_menu_options[0]);

	static unsigned char menu_index = 0;

	print_two_lines_delay_1s(PSTR("   Main Menu"),0);

	while(1)
	{
		clear();
		lcd_goto_xy(0,1);

		print_from_program_space(PSTR("\x7ftop  \xa5mid  bot\x7e"));
		lcd_goto_xy(0,0);
		print_from_program_space(main_menu_options[menu_index]);

		// wait for all buttons to be released, then a press
		while(button_is_pressed(ALL_BUTTONS));
		unsigned char button = wait_for_button_press(ALL_BUTTONS);

		if(button & TOP_BUTTON)
		{
			//wait_for_button_release(TOP_BUTTON);// tmphax to get it to work on org06a01
			play_from_program_space(beep_button_top);

			if (menu_index == 0)
			{
				menu_index = main_menu_length-1;
			}
			else
			{
				menu_index --;
			}
		}
		else if(button & MIDDLE_BUTTON)
		{
			clear();

			play_from_program_space(beep_button_middle);
			wait_for_button_release(button);

			while(!button_is_pressed(MIDDLE_BUTTON))
			{
				lcd_goto_xy(0,1);
				print_from_program_space(PSTR("\6mid"));
				lcd_goto_xy(0,0);
				main_menu_functions[menu_index]();
			}

			set_motors(0,0);
			stop_playing();
			m1_speed = 0;
			m2_speed = 0;
			red_led(0);
			green_led(0);
			lcd_hide_cursor();
			serial_cancel_receive(USB_COMM);
			play_from_program_space(beep_button_middle);
		}
		else if(button & BOTTOM_BUTTON)
		{
			play_from_program_space(beep_button_bottom);

			if (menu_index == main_menu_length-1)
			{
				menu_index = 0;
			}
			else
			{
				menu_index ++;
			}
		}
	}
}

/******************************************************************************/
/* Factory test code accessed by holding down a button on startup             */

// Error codes
#define ERROR_CURRENT_SENSE_1_LOW  0b10000010
#define ERROR_CURRENT_SENSE_1_HIGH 0b10000100
#define ERROR_CURRENT_SENSE_2_LOW  0b10000110
#define ERROR_CURRENT_SENSE_2_HIGH 0b10001000
#define ERROR_D0_LOW               0b10001010
#define ERROR_D2_LOW               0b10001100
#define ERROR_C5_LOW               0b10001110
#define ERROR_C5_HIGH              0b10010000
#define ERROR_D2_HIGH              0b10010010
#define ERROR_D4_HIGH              0b10010100
#define ERROR_B1_LOW               0b10010110
#define ERROR_B1_HIGH              0b10011000
#define ERROR_D5_LOW               0b10011010
#define ERROR_C1_LOW               0b10011100
#define ERROR_B4_LOW               0b10011110
#define ERROR_C3_LOW               0b10100000
#define ERROR_C3_HIGH              0b10100010
#define ERROR_A4_HIGH              0b10100011
#define ERROR_A4_LOW               0b10100100
#define ERROR_A6_HIGH              0b10100101
#define ERROR_A6_LOW               0b10100110
#define ERROR_SHUTDOWN             0b10100111

void all_inputs()
{
	// Make all pins inputs.
	DDRA = DDRB = DDRC = DDRD = 0;

	// Disable all pull-up resistors.
	PORTA = PORTB = PORTC = PORTD = 0;
}

// Drive all the lines low which are safe to drive.
void drive_all_low()
{
	// Make all pins inputs right now so that all pins will be in a known state
	// at the end of this function, and so that we don't cause momentary shorts
	// when changing output states.
	all_inputs();

	// Motor control lines
	set_digital_output(IO_D7, LOW);
	set_digital_output(IO_D6, LOW);
	set_digital_output(IO_C7, LOW);
	set_digital_output(IO_C6, LOW);

	// Demux control lines
	set_digital_output(IO_D5, LOW); // SPWM / pic RX
	set_digital_output(IO_D0, LOW);
	set_digital_output(IO_C1, LOW);
	set_digital_output(IO_D2, LOW);

	set_digital_output(IO_D4, LOW); // Buzzer

	// Button to end demux test
	set_digital_output(IO_B0, LOW);
	set_digital_output(IO_B2, LOW);

	set_digital_output(IO_A0, LOW); // VADJ current divider

	set_digital_output(IO_C4, LOW); // green LED

	set_digital_output(IO_D1, LOW); // red LED

	set_digital_output(IO_B4, LOW); // backlight (with external pull-up, good)

	// A4 connected to B1
	set_digital_output(IO_B1, LOW);
	set_digital_output(IO_A4, LOW);

	// Misc
	set_digital_output(IO_A3, LOW);
	set_digital_output(IO_A5, LOW);
	set_digital_output(IO_A7, LOW);

	set_digital_output(IO_B3, LOW);

	set_digital_output(IO_C2, LOW);
	set_digital_output(IO_C5, LOW);

	// Pins it is NOT safe to drive:
	// A1, A2: connected to current sense lines.
	// A6: connected to PD4bar
	// C3: connected to K
	// C0: connected to shutdown (off) pin so can't drive it low until the end
}

// A fatal error has occurred, so blink the red LED forever.
void error_red_led()
{
	all_inputs();
	while(1)
	{
		red_led(1);
		green_led(0);
		delay_ms(250);
		red_led(0);
		green_led(1);
		delay_ms(250);
	}
}

void turn_on_led(unsigned char number)
{
	set_digital_output(IO_D5, HIGH);
	set_digital_output(IO_D0, (number & 1) ? HIGH : LOW);
	set_digital_output(IO_C1, (number & 2) ? HIGH : LOW);
	set_digital_output(IO_D2, (number & 4) ? HIGH : LOW);
}

void turn_off_leds()
{
	set_digital_output(IO_D5, LOW);
}

void flash_leds(unsigned char leds)
{
	unsigned char i;
	for(i=0; i < 8; i++)
	{
		if (leds >> i & 1)
		{
			turn_on_led(i);
			delay_ms(2);
		}
	}
}

void show_leds(unsigned char leds, unsigned short time)
{
	time_reset();
	while(get_ms() < time){ flash_leds(leds); }
}

void error(unsigned char leds)
{
	all_inputs();
	while(1){ flash_leds(leds); }
}

// jig_test: make sure that the board is on the test fixture.
// (This test code should be run on the test fixture with no LCD
//  attached.)
// At this point, we want to drive as few lines as possible because
// we don't know what kind of hardware the board is connected to and
// we don't want to damage anything.  Any of the lines that we DO drive
// should be lines that the user is unlikely to hook up to anything.
void jig_test()
{
	all_inputs(); // DON'T drive all low until we  know we're on the jig

	// Make sure PD3 is connected to PC5.
	set_digital_input(IO_D3, PULL_UP_ENABLED);
	delay_ms(2);
	if (!is_digital_input_high(IO_D3)){	error_red_led(); }
	set_digital_output(IO_C5, LOW);
	delay_ms(1);
	if (is_digital_input_high(IO_D3)){ error_red_led(); }
}

// demux_test: Flash the 8 LEDs connected to the demultiplexer
// in sequence.  PD5 -> SPWM,  PD0 -> SA, PC1 -> SB, PD2 -> SC.
// PB2 -> Button -> PB0
void demux_test()
{
	unsigned char led = 7;

	drive_all_low();
	set_digital_output(IO_C0, LOW); // connected to shutdown pin, so can't drive it low!

	while(1)
	{
		turn_on_led(led);

		// Select the next LED.
		if (led == 0) { led = 7; }
		else { led--; }

		time_reset();
		while(get_ms() < 180)
		{
			// Check to see if B0 is connected to B2.
			set_digital_input(IO_B0, PULL_UP_ENABLED);

			set_digital_output(IO_B2, LOW);
			delay_ms(1);
			if (is_digital_input_high(IO_B0)){ continue; }

			set_digital_output(IO_B2, HIGH);
			delay_ms(1);
			if (!is_digital_input_high(IO_B0)){ turn_on_led(2); continue; }

			set_digital_input(IO_B2, PULL_UP_ENABLED);

			set_digital_output(IO_B0, HIGH);
			delay_ms(1);
			if (!is_digital_input_high(IO_B2)){ turn_on_led(3); continue; }

			set_digital_output(IO_B0, LOW);
			delay_ms(1);
			if (is_digital_input_high(IO_B2)){ turn_on_led(4); continue; }

			while(!is_digital_input_high(IO_B2)); // Wait for button release.
			return; // Success
		}
	}
}

// Connections: VADJ -- 4.6k -- PA0 -- 2.2k -- GND
//
//                  Max     Min
// VADJ             9.0 V   2.5 V
// PA0              3.0 V   .83 V
// reading          153     42     (ADC offset error not accounted for)
// (reading-50)/10  10.3    0
//
// For this test to the succeed, the user must turn the VADJ pot from
// one extreme to the other.  The 8 LEDs on the MUX give the user feedback
// about the current VADJ level and which levels he needs to reach.
void vadj_test()           
{
	set_analog_mode(MODE_8_BIT);

	unsigned char levels_reached = 0;

	drive_all_low();
	set_digital_input(IO_A0, HIGH_IMPEDANCE);
	delay_ms(2);	

	while(1)
	{
		// Compute what level the VADJ is on a scale of 0-7.
		unsigned char level = analog_read(0);
		if (level < 50){ level = 0; }
		else { level = (level - 50)/10; }
		if (level > 7){ level = 7; }

		levels_reached |= 1<<level;  // Record this level.
		if(levels_reached == 0xFF)
		{
			return; // Succes!
		}

		// Show the levels on the 8 LEDs.
		// LED off: level not reached.
		// LED on dim: level reached.
		// LED on bright: current level.
		for(unsigned char i=0; i < 8; i++)
		{
			if ((levels_reached >> i) & 1){ turn_on_led(i); }
			else { turn_off_leds(); }
			delay_ms((i == level) ? 8 : 2);
		}

	}
}

void gradual_set_motors(signed int target1, signed int target2)
{
	static signed int speed1 = 0;
	static signed int speed2 = 0;

	while(speed1 != target1 || speed2 != target2)
	{
		if (target1 > speed1){ speed1 += 1; }
		else if (target1 < speed1){ speed1 -= 1; }

		if (target2 > speed2){ speed2 += 1; }
		else if (target2 < speed2){ speed2 -= 1; }

		set_motors(speed1, speed2);

		delay_ms(2);
	}

}

void assert_current_sense(unsigned char cs1, unsigned char cs2)
{
	switch(cs1)
	{
		case LOW:  if (analog_read(1) > 9){ error(ERROR_CURRENT_SENSE_1_HIGH); } break;
		case HIGH: if (analog_read(1) < 15){ error(ERROR_CURRENT_SENSE_1_LOW); } break;
	}

	switch(cs2)
	{
		case LOW:  if (analog_read(2) > 9){ error(ERROR_CURRENT_SENSE_2_HIGH); } break;
		case HIGH: if (analog_read(2) < 15){ error(ERROR_CURRENT_SENSE_2_LOW); } break;
	}

}

// Both motor outputs should be hooked up to motors that can draw at least
// 500 mA of current when driven at 9 V.  
// CS1 -> A1
// CS2 -> A2
void factory_motor_test()
{
	return; // TMPHAX: return because we don't have any motors yet

	drive_all_low();
	set_analog_mode(MODE_8_BIT);

	assert_current_sense(LOW, LOW);

	// Test all 9 combinations of motor speed.
	gradual_set_motors( 255,    0); assert_current_sense(HIGH, LOW );
	gradual_set_motors( 255,  255); assert_current_sense(HIGH, HIGH);
	gradual_set_motors(  0,   255); assert_current_sense(LOW,  HIGH);
	gradual_set_motors(-255,  255); assert_current_sense(HIGH, LOW );
	gradual_set_motors(-255,    0); assert_current_sense(HIGH, LOW );
	gradual_set_motors(-255, -255); assert_current_sense(HIGH, HIGH);
	gradual_set_motors(  0,  -255); assert_current_sense(LOW,  HIGH);
	gradual_set_motors(255,  -255); assert_current_sense(HIGH, HIGH);
	gradual_set_motors(  0,     0); assert_current_sense(LOW,  LOW );

	delay_ms(100);
}

void input_test()
{
	// Pin 1: MOSI: tested when we communicate with aux. processor
	// Pin 2: MISO: tested when we communicate with aux. processor
	// Pin 3: SCK: tested when we communicate with aux. processor
	// Pin 4: RESET: tested when the AVR is programmed
	// Pin 5: VCC
	// Pin 6: GND
	// Pin 7: XTAL1: tested whenever the AVR is running
	// Pin 8: XTAL2: tested whenever the AVR is running

	// Pin 9: PD0: connected to SA and A and tested during demux test
	drive_all_low();
	set_digital_input(IO_D0, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_D0)){ error(ERROR_D0_LOW); }

	// TODO: Pin 10: PD1
		
	// Pin 11: PD2: connected to SC and C and tested during demux test
	drive_all_low();
	set_digital_input(IO_D2, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_D2)){ error(ERROR_D2_LOW); }

	// Pin 12: PD3 connected to PC5 and already tested a little bit in jig_test
	// Pin 24: PC5
	drive_all_low();
	set_digital_input(IO_C5, PULL_UP_ENABLED);
	set_digital_input(IO_D3, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_D3)){ error(ERROR_D2_LOW); }
	if (!is_digital_input_high(IO_C5)){ error(ERROR_C5_LOW); }
	set_digital_output(IO_C5, LOW);
	delay_ms(1);
	if (is_digital_input_high(IO_D3)){ error(ERROR_D2_HIGH); }
	set_digital_input(IO_C5, PULL_UP_ENABLED);
	set_digital_output(IO_D3, LOW);
	delay_ms(1);
	if (is_digital_input_high(IO_C5)){ error(ERROR_C5_HIGH); }

	// Pin 13: PD4 connected to buzzer MOSFET input
	// Pin 31: PA6 connected to PD4bar
	drive_all_low();
	set_digital_input(IO_D4, HIGH_IMPEDANCE);  // D4 gets externally pulled down
	set_digital_input(IO_A6, PULL_UP_ENABLED);
	delay_ms(5);
	if (is_digital_input_high(IO_D4)){ error(ERROR_D4_HIGH); }
	if (!is_digital_input_high(IO_A6)){ error(ERROR_A6_LOW); }
	set_digital_output(IO_D4, HIGH);
	delay_ms(1);
	if (is_digital_input_high(IO_A6)){ error(ERROR_A6_HIGH); }

	// Pin 14: PD5 connected to SPWM/RX
	drive_all_low();
	set_digital_input(IO_D5, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_D5)){ error(ERROR_D5_LOW); }

	// Pin 15: PD6/PWM2: tested as output in motor test
	// Pin 16: PD7/PWM2: tested as output in motor test

	// Pin 17: VCC
	// Pin 18: GND

	// TODO: Pin 19: PC0

	// Pin 20: PC1 connected to SB and tested in demuxTest
	drive_all_low();
	set_digital_input(IO_C1, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_C1)){ error(ERROR_C1_LOW); }

	// TODO: Pin 21: PC2: connected to the pushbutton 1k pulldown
	
	// Pin 22: PC3: connected to K
	// Pin 44: PB4: connected to backlight MOSFET input
	drive_all_low();
	set_digital_input(IO_B4, HIGH_IMPEDANCE);  // B4 externally pulled up
	set_digital_input(IO_C3, PULL_UP_ENABLED);
	delay_ms(1);
	if (!is_digital_input_high(IO_B4)){ error(ERROR_B4_LOW); }
	if (is_digital_input_high(IO_C3)){ error(ERROR_C3_HIGH); }
	set_digital_output(IO_B4, LOW);
	delay_ms(1);
	if (!is_digital_input_high(IO_C3)){ error(ERROR_C3_LOW); }

	// TODO: Pin 23: PC4

	// Pin 24: see above
	// Pin 25: PC6/DIR2: tested as output in motor_test
	// Pin 26: PC6/DIR2: tested as output in motor_test
	// Pin 27: AVCC: tested whenever we take an ADC reading
	// Pin 28: GND

	// TODO: Pin 29: AREF

	// TODO: Pin 30: PA7

	// Pin 31: see above

	// TODO: Pin 32: PA5

	// Pin 33: PA4 connected to pin PB1, which has an external 4.7k pull-down resistor
	// Pin 41: PB1
	drive_all_low();
	set_digital_input(IO_A4, HIGH_IMPEDANCE);
	set_digital_input(IO_B1, PULL_UP_ENABLED);
	if (is_digital_input_high(IO_A4)){ error(ERROR_A4_HIGH); }
	if (is_digital_input_high(IO_B1)){ error(ERROR_B1_HIGH); }
	set_digital_output(IO_B1, HIGH);
	delay_ms(1);
	if (!is_digital_input_high(IO_A4)){ error(ERROR_A4_LOW); }
	set_digital_input(IO_B1, HIGH_IMPEDANCE);
	set_digital_output(IO_A4, HIGH);
	delay_ms(1);
	if (!is_digital_input_high(IO_B1)){ error(ERROR_B1_LOW); }

	// TODO: Pin 34: PA3

	// Pin 35: PA2: connected to CS2, tested as input in motor_test
	// Pin 36: PA1: connected to CS1, tested as input in motor_test
	// Pin 37: PA0: connected to VADJ/3, tested as input in vadj_test
	// Pin 38: VCC
	// Pin 39: GND
	// Pin 40: PB0: tested in demuxTest

	// TODO: Pin 41: PB1

	// Pin 42: PB2: tested in demuxTest

	// TODO: Pin 43: PB3

	// Pin 44: see above

	// TODO: test aux. processor input A
	// TODO: test aux. processor input B
	// TODO: test aux. processor input C
	// TODO: test aux. processor input D
}

void shutdown_test()
{
	all_inputs();

	// Flash LEDs 3 times
	show_leds(0xFF, 170);
	turn_off_leds();
	delay_ms(170);
	show_leds(0xFF, 170);
	turn_off_leds();
	delay_ms(170);
	show_leds(0xFF, 170);
	
	// Turn off the power supply
	set_digital_input(IO_C0, PULL_UP_ENABLED);

	while(1){ flash_leds(ERROR_SHUTDOWN); }
}

void test()
{
	jig_test();

	demux_test();

	factory_motor_test();

	input_test();

	vadj_test();

	shutdown_test();
}

/* End of factory test code.                                                  */
/******************************************************************************/


// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	test(); // tmphax

	// If any button is pressed, go into the factory test code.
	if(button_is_pressed(BOTTOM_BUTTON))
		test();

	clear();
	print_two_lines_delay_1s(PSTR("     Pololu     "),
	                         PSTR(" Orangutan SVP  "));

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
