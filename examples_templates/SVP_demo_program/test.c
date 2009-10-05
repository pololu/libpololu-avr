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

/**********************************************************************/
/* Simpler test code accessed by holding down a button on startup     */

// store this fugue in program space using the PROGMEM macro.     
// Later we will play it directly from program space using the melodyTest()
// function, bypassing the need to load it all into RAM first.
const char fugue[] PROGMEM =    
	"! O5 L16 agafaea dac+adaea fa<aa<bac#a dac#adaea f"    
	"O6 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"    
	"L8 MS <b-d<b-d MLe-<ge-<g MSc<ac<a ML d<fd<f O5 MS b-gb-g"    
	"ML >c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"    
	"O6 L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"    
	"<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"    
	"<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"    
	"O5 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"    
	"e>d>c#>db>d>c#b >c#agaegfe f O6 dc#dfdc#<b c#4";




// *** triggered by top button ***
// This function plays a melody from flash in the background while the two
// user LEDs alternately fade in and out.
unsigned char melodyTest()
{
	unsigned char button;
	int i = 0;

	// the following function does not block execution
	play_from_program_space(fugue);	// play music from flash in the background

	red_led(0);		// turn red and green LEDs off
	green_led(0);

	clear();			// clear the LCD, go to the start of the first LCD line
	print("melody:");	// print to the LCD
	lcd_goto_xy(0, 1);	// go to the start of the second LCD line
	print("fugue");		// print to the LCD

	time_reset();		// reset the internal millisecond timer count to zero

	while (1)			// loop here until we detect a button press and return
	{
		if (get_ms() >= 5)	// if 5 or more milliseconds have elapsed
		{
			time_reset();	// reset timer count to zero
			// check if middle or bottom buttons have been pressed
			button = button_is_pressed(MIDDLE_BUTTON | BOTTOM_BUTTON);
			if (button != 0)	// if so, stop melody and return the button ID
			{
				stop_playing();	// stop playing music
				return button;
			}
			i += 5;		// increase our state variable based on the time
			if (i >= 1000)	// once a second has elapsed, reset the state var
				i = 0;
		}

		// the following code alternately flashes the red and green LEDs,
		// fading them in and out over time as the music plays in the
		// background.  This is accomplished by using high-frequency PWM
		// signals on the LED lines.  Essentially, each LED is flickering
		// on and off very quickly, which gives it the apperance of variable
		// brightness depending on the percentage of the cycle the LED is on.
		// Each LED flicker cycle takes a total approximately 251 us.

		if (i < 250)		// phase 1: ramp up the red LED brightness
		{					//  as i increases over time
			red_led(1);			// turn the red LED on
			delay_us(i + 1);	// microsecond delay
			red_led(0);			// turn the red LED off
			delay_us(250 - i);	// microsecond delay
		}
		else if (i < 500)	// phase 2: ramp down the red LED brightness
		{
			red_led(1);			// turn the red LED on
			delay_us(500 - i);	// microsecond delay
			red_led(0);			// turn the red LED off
			delay_us(i - 249);	// microsecond delay
		}
		else if (i < 750)	// phase 3: ramp up the green LED brightness
		{
			green_led(1);		// turn the green LED on
			delay_us(i - 499);	// microsecond delay
			green_led(0);		// turn the green LED off
			delay_us(750 - i);	// microsecond delay
		}
		else				// phase 4: ramp down the green LED brightness
		{
			green_led(1);		// turn the green LED on
			delay_us(1000 - i);	// microsecond delay
			green_led(0);		// turn the green LED off
			delay_us(i - 749);	// microsecond delay
		}
		
	}
}


// *** triggered by middle button ***
// This function tests the eight user I/O pins and the trimmer potentiometer.
// At any given time, one pin is being driven low while the rest are weakly
// pulled high (to 5 V).  At the same time, the LCD is displaying the input
// values on the eight user I/O pins.  If you short a pin to *GROUND* (note:
// do not short the pin to power, only short it to one of the ground pads
// along the top edge of the board), you will see the corresponding bit on
// the LCD go to zero.  The PD1 bit will always read zero as it is being
// pulled down through the red user LED.
unsigned char IOTest()
{
	// the bits of the "outputs" byte will correspond to the pin states of
	// the user I/O lines as follows:

	// outputs:   b7  b6  b5  b4  b3  b2  b1  b0
	// user I/O: PC5 PC4 PC3 PC2 PC1 PC0 PD1 PD0

	// Only one bit of "outputs" will ever be set (1) at a time; the rest will
	// be cleared (0).  The user I/O pin that corresponds to the set bit will
	// be an output that is driven low while all of the other user I/O pins
	// will be inputs with internal pull-ups enabled (i.e. they will be weakly
	// pulled to 5V and will read as high).
	unsigned char outputs = 0x80;	// binary: 10000000
	unsigned char direction = 0;
	unsigned char button;

	red_led(0);		// turn red and green LEDs off
	green_led(0);

	clear();		// clear the LCD
	print("User I/O");

	set_analog_mode(MODE_8_BIT);	// configure ADC for 8-bit readings

	while (1)			// loop here until we detect a button press and return
	{
		time_reset();				// reset millisecond timer count to zero

		DDRC = 0;					// make PC0 - PC5 inputs
		PORTC = 0;					// PC0 - PC5 -> high impedance inputs
		DDRD &= ~0x03;				// clear PD0 & PD1 bits (make them inputs)
		PORTD &= ~0x03;				// PD0 & PD1 -> high impedance inputs
		PORTC |= ~outputs >> 2;		// set the outputs states of PC0 - PC5
		DDRC |= outputs >> 2;		// make low pin an output (inputs for rest)
		PORTD |= ~outputs & 0x03;	// set the output states of PD0 and PD1
		DDRD |= outputs & 0x03;		// make low pin an output (inputs for rest)

		// The following loop will execute for an amount of time determined
		// by the position of the user trimmer potentiometer (trimpot).
		// When the trimpot is at one extreme, the loop will take 256*2 = 512
		// milliseconds.  When the trimpot is at the other extreme, the
		// loop will only execute once, which takes slightly more than 20 ms.
		// In this way, the trimpot controls the speed at which the output
		// byte changes.
		do
		{
			// The bits of the "inputs" byte reflect the input values of pins
			// PD0, PD1, and PC0 - PC5.  Bit 0 corresponds to PD0, bit 1 to
			// PD1, and bits 2 - 7 to PC0 - PC5, respectively.
			unsigned char inputs = PIND & 0x03;	// store PD0 and PD1 input vals
			inputs |= PINC << 2;	// store PC0 - PC5 input values
			lcd_goto_xy(0, 1);		// go to the start of the second LCD line
			print_binary(inputs);	// print the "input" byte in binary
			delay_ms(20);			// delay here for 20 milliseconds
			// check if top or bottom buttons have been pressed
			button = button_is_pressed(TOP_BUTTON | BOTTOM_BUTTON);
			if (button != 0)	// if so, reset I/O states, return button ID
			{
				DDRC = 0;		// make PC0 - PC5 inputs
				PORTC = 0;		// disable pull-ups on PC0 - PC5
				DDRD &= ~0x03;	// make PD0 and PD1 inputs
				PORTD &= ~0x03;	// disable pull-ups on PD0 and PD1
				return button;
			}
		}
		while (get_ms() < read_trimpot() * 2);

		if (direction)
			outputs <<= 1;		// bit-shift our output byte left by one bit
		else
			outputs >>= 1;		// bit-shift our output byte right by one bit

		if (outputs == 1)		// if we have reached the right edge
			direction = 1;		// switch direction to "left"
		if (outputs == 0x80)	// if we have reached the left edge
			direction = 0;		// switch direction to "right"
	}
}


// This function comprises the main part of the motor speed update loop.
// If a button press is detected, both the red and green user LEDs are
// turned off, as are the motor outputs.  This function also includes
// a brief delay that ensures the entire loop takes the desired amount
// of time.
unsigned char motorUpdate(unsigned char motor, int speed)
{
	unsigned char button;

	if (motor == 0)		// set the desired motor to the desired speed
		set_m1_speed(speed);
	else
		set_m2_speed(speed);

	delay_ms(2);		// delay here for 2 milliseconds

	// check if top or bottom buttons have been pressed
	button = button_is_pressed(TOP_BUTTON | MIDDLE_BUTTON);
	if (button != 0)	// if so, turn off motors and LEDs, return button ID
	{
		red_led(0);
		green_led(0);
		set_motors(0, 0);
	}
	return button;
}


// *** triggered by middle button ***
// This function tests the motors by first ramping motor1 speed from zero
// to full speed "forward", to full speed "reverse", and finally back to zero.
// It then does the same for motor2 before repeating all over again.
// While motor1 is running, the red user LED is on, otherwise it is off.
// While the currently active motor is moving "forward", the green user LED
// is on, otherwise it is off.  The LCD gives you feedback as to which motor
// is currently moving in which direction (F = "forward", R = "reverse", and
// - = inactive).
unsigned char motorTest()
{
	unsigned char button;
	int speed;
	unsigned char motor = 0;

	clear();			// clear the LCD, go to the start of the first LCD line
	print("motor2");	// print to the first line of the LCD
	lcd_goto_xy(0, 1);	// go to the start of the second LCD line
	print("motor1");	// print to the second line of the LCD

	while (1)
	{
		red_led(!motor);	// turn red LED on when m1 is active, off for m2
		lcd_goto_xy(7, !motor);	// go to end of LCD line for active motor
		print("F");				// print "F" for "forward"
		lcd_goto_xy(7, motor);	// go to end of LCD line for inactive motor
		print("-");				// print "-" for "inactive"
		green_led(1);		// turn green LED on when motor is moving "forward"
		for (speed = 0; speed < 255; speed++)
		{
			button = motorUpdate(motor, speed);	// ramp up motor speed
			if (button != 0)					//  from 0 to 255
				return button;
		}

		for (speed = 255; speed > -255; speed--)
		{
			if (speed == -1)	// motor starts moving in "reverse"
			{
				green_led(0);	// green LED off when motor going "reverse"
				lcd_goto_xy(7, !motor);	// go to end of active motor's LCD line
				print("R");		// print "R" for "reverse"
			}

			button = motorUpdate(motor, speed);	// ramp down motor speed
			if (button != 0)					//  from 255 to -255
				return button;
		}

		for (speed = -255; speed <= 0; speed++)
		{
			button = motorUpdate(motor, speed);	// ramp up motor speed
			if (button != 0)					//  from -255 to 0
				return button;
		}

		motor = !motor;		// alternate between m1 and m2
	}
}

void test()
{
	unsigned char button;

	clear();
	delay(200);
	print("Orangutn");	// print to the top line of the LCD
	delay_ms(400);		// delay 200 ms
	lcd_goto_xy(0, 1);	// go to the start of the second LCD line
	print(" SVP");	    // print to the bottom line of the LCD

	delay_ms(1000);		// delay 700 ms

	clear();			// clear the LCD, move cursor to start of top line

	print("  VBAT");

	do
	{
		// Perform 10-bit analog-to-digital conversions on ADC channel 6.
		// Average ten readings and return the result, which will be one
		// third of the battery voltage when the "ADC6 = VBAT/3" solder
		// bridge is in place on the bottom of the Orangutan PCB
		int vbat = analog_read_average(6, 10);	// 10-sample avg of ADC6
		vbat = to_millivolts(vbat) * 3;	// convert reading to bat. voltage (mV)
		lcd_goto_xy(0, 1);	// go to the start of the second LCD line
		print_long(vbat);	// display battery voltage in millivolts
		print(" mV ");		// display the units
		delay_ms(50);		// delay for 50 ms
		button = button_is_pressed(ALL_BUTTONS);	// check for button press
	}
	while (button == 0);	// loop if no buttons are being pressed


	// *** MAIN LOOP ***

	while (1)	// loop forever
	{
		if (button & TOP_BUTTON)			// if the top button is pressed
			button = melodyTest();	// this func. loops until next button press

		else if (button & MIDDLE_BUTTON)	// if the middle button is pressed
			button = IOTest();		// this func. loops until next button press

		else if (button & BOTTOM_BUTTON)	// if the bottom button is pressed
			button = motorTest();	// this func. loops until next button press
	}
}

/* End of simpler test code.                                          */
/**********************************************************************/


// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	clear();
	print_two_lines_delay_1s(PSTR("     Pololu     "),
	                         PSTR(" Orangutan SVP  "));

	// if any button is pressed, go into the old version of the test code
	if(button_is_pressed(ALL_BUTTONS))
		test(); // activate the simpler test code

	// set up the robot
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
