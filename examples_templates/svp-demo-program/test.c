/*
 * svp-demo-program - demo code that ships with the Pololu Orangutan SVP
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

#define LCD_WIDTH 16

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
	static const char note[] PROGMEM = {
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
	static const char back_arrow[] PROGMEM = {
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

	for(i = 0; i < LCD_WIDTH; i++)
	{
		print_character(pgm_read_byte(music_title + title_pos + i));
	}

	title_pos++;
	if(title_pos + LCD_WIDTH >= sizeof(music_title))
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

	char screen_string[LCD_WIDTH+1] = "Type in to COM.";
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

				if (screen_string_length == LCD_WIDTH)
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
	unsigned char button = wait_for_button_press(ANY_BUTTON);
	
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
		while(button_is_pressed(ANY_BUTTON));
		unsigned char button = wait_for_button_press(ANY_BUTTON);

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

void start_blinking_red_led()
{
    // Configure Timer 0 to overflow 76 times per second and enable the interrupt.
	TCCR0A = 0;
	TCCR0B = 0x05; // Prescaler 1:1024: (20MHz/1024/256) = 76 Hz
	TIMSK0 = 1<<TOIE0; // Enable the overflow interrupt.
	sei();
}

// This interrupt service routine is called each time Timer 0 overflows.
// It takes care of blinking the LED app
ISR(TIMER0_OVF_vect)
{
	static unsigned char counter = 0;
	counter++;
	if (counter == 1){ red_led(1); }
	if (counter == 35){ red_led(0); }
	if (counter == 70){ counter = 0; }
}

void stop_blinking_red_led()
{
	TIMSK0 = 0; // Disable the Timer 0 interrupts.
	red_led(0); // Turn off the red led.
}

// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	start_blinking_red_led();

	// Make sure that this program does not get run on the factory test fixture.
	wait_for_button_release(BOTTOM_BUTTON);

	green_led(1);

	load_custom_characters();
	
	play_from_program_space(welcome);

	#if defined(__AVR_ATmega324P__) || defined(__AVR_ATmega324PA__)
	print_two_lines_delay_1s(PSTR("Pololu Orangutan"),
	                         PSTR("    SVP-324     "));
    #else
	print_two_lines_delay_1s(PSTR("Pololu Orangutan"),
	                         PSTR("    SVP-1284    "));
    #endif

	print_two_lines_delay_1s(PSTR("Demo Program    "), 0);

	print_two_lines_delay_1s(PSTR("Use mid button  "),
							 PSTR("to select.      "));

	clear();
	print_from_program_space(PSTR("Press middle    "));
	lcd_goto_xy(0,1);
	print_from_program_space(PSTR("button: Try it! "));

	while(!(wait_for_button_and_beep() & MIDDLE_BUTTON));

	stop_blinking_red_led();

	green_led(0);

	play_from_program_space(thank_you_music);

	print_two_lines_delay_1s(0, PSTR("   Thank you!   "));

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
