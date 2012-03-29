/*
 * x2-demo-program - demo code for the Pololu Orangutan X2 robot
 * controllers.
 * 
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
// Orangutan X2 has much more program space than RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";

const char welcome_line2[] PROGMEM = " Orangutan X2";

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
const char menu_motor_test[] PROGMEM = "Motors";
const char menu_music_test[] PROGMEM = "Music";
const char menu_pot_test[] PROGMEM = "Trimpot";
const char menu_time_test[] PROGMEM = "Timer";

const char menu_line2[] PROGMEM = "\x7f" "A \xa5" "B C\x7e";
const char back_line2[] PROGMEM = "\6B";

void bat_test();
void led_test();
void lcd_test();
void motor_test();
void music_test();
void time_test();
void pot_test();
typedef void (*function)();
const function main_menu_functions[] = { bat_test, led_test, pot_test, motor_test, music_test, time_test };
const char *main_menu_options[] = { menu_bat_test, menu_led_test, menu_pot_test, menu_motor_test, menu_music_test, menu_time_test };
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
	int bat = read_battery_millivolts_x2();

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
	print("Red 1   ");

	red_led(1);
	if(wait_for_250_ms_or_button_b())
		return;
	red_led(0);
	if(wait_for_250_ms_or_button_b())
		return;

	play(">c32");
	lcd_goto_xy(0,0);
	print("Green 1 ");

	green_led(1);
	if(wait_for_250_ms_or_button_b())
		return;
	green_led(0);
	if(wait_for_250_ms_or_button_b())
		return;

	play("d32");
	lcd_goto_xy(0,0);
	print("Red 2   ");

	red_led2(1);
	if(wait_for_250_ms_or_button_b())
		return;
	red_led2(0);
	if(wait_for_250_ms_or_button_b())
		return;

	play(">d32");
	lcd_goto_xy(0,0);
	print("Green 2 ");

	green_led2(1);
	if(wait_for_250_ms_or_button_b())
		return;
	green_led2(0);
	if(wait_for_250_ms_or_button_b())
		return;

	play("e32>e32");
	lcd_goto_xy(0,0);
	print("Yellow  ");

	yellow_led(1);
	if(wait_for_250_ms_or_button_b())
		return;
	yellow_led(0);
	if(wait_for_250_ms_or_button_b())
		return;
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

const char rhapsody[] PROGMEM = "O6 T40 L16 d#<b<f#<d#<f#<bd#f#"
  "T80 c#<b-<f#<c#<f#<b-c#8"
  "T180 d#b<f#d#f#>bd#f#c#b-<f#c#f#>b-c#8 c>c#<c#>c#<b>c#<c#>c#c>c#<c#>c#<b>c#<c#>c#"
  "c>c#<c#>c#<b->c#<c#>c#c>c#<c#>c#<b->c#<c#>c#"
  "c>c#<c#>c#f>c#<c#>c#c>c#<c#>c#f>c#<c#>c#"
  "c>c#<c#>c#f#>c#<c#>c#c>c#<c#>c#f#>c#<c#>c#d#bb-bd#bf#d#c#b-ab-c#b-f#d#";

const char rhapsody_title[] PROGMEM = "       \7 Hungarian Rhapsody No. 2 - by Franz Liszt \7       ";

void music_test()
{
	static char rhapsody_title_pos = 0;
	static long last_shift = 0;
	char c,i;

	if(get_ms() - last_shift > 250)
	{
		for(i=0;i<8;i++)
		{
			c = pgm_read_byte(rhapsody_title + rhapsody_title_pos + i);
			print_character(c);
		}
		last_shift = get_ms();

		rhapsody_title_pos ++;
		if(rhapsody_title_pos + 8 >= sizeof(rhapsody_title))
			rhapsody_title_pos = 0;
	}

	if(!is_playing())
	{
		play_from_program_space(rhapsody);
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

// Initializes, displays a welcome message, calibrates, and
// plays the initial music.
void initialize()
{
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
	unsigned int outputs = 0x8000;	// binary: 10000000 00000000
	unsigned char *outputsA = (unsigned char *)&outputs;	// pointer to low byte of outputs
	unsigned char *outputsD = outputsA + 1;					// pointer to high byte of outputs
	unsigned char direction = 0;
	unsigned char button;

	red_led(1);
	green_led(1);

	clear();		// clear the LCD
	print("User I/O");
	lcd_goto_xy(0, 1);
	print("DDDDDDDD AAAAAAAA");
	lcd_goto_xy(0, 2);
	print("76543210 76543210");

	set_analog_mode(MODE_8_BIT);	// configure ADC for 8-bit readings

	while (1)			// loop here until we detect a button press and return
	{
		time_reset();				// reset millisecond timer count to zero

		DDRA = 0;					// make PA0 - PA7 inputs
		PORTA = 0;					// PA0 - PA7 -> high impedance inputs
		DDRD = 0;					// make PD0 - PD7 inputs
		PORTD = 0;					// PD0 - PD7 -> high impedance inputs

		PORTD |= ~(*outputsD);
		DDRD |= *outputsD;
		PORTA |= (~(*outputsA)) & 0x3F;
		DDRA |= *outputsA & 0x3F;	// never make PA6 and PA7 outputs


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
			unsigned char inputsA = PINA;
			unsigned char inputsD = PIND;
			lcd_goto_xy(0, 3);		// go to the start of the fourth LCD line
			print_binary(inputsD);	// print the "input" byte in binary
			print(" ");
			print_binary(inputsA);
			delay_ms(20);			// delay here for 20 milliseconds
			// check if top or bottom buttons have been pressed
			button = button_is_pressed(TOP_BUTTON | BOTTOM_BUTTON);
			if (button != 0)	// if so, reset I/O states, return button ID
			{
				DDRA = 0;					// make PA0 - PA7 inputs
				PORTA = 0;					// PA0 - PA7 -> high impedance inputs
				DDRD = 0;					// make PD0 - PD7 inputs
				PORTD = 0;					// PD0 - PD7 -> high impedance inputs
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
		if (outputs == 0x8000)	// if we have reached the left edge
			direction = 0;		// switch direction to "right"
	}
}



unsigned char motor_wait(unsigned int time_ms)
{
	unsigned char button;

	unsigned long time = get_ms();

	while (get_ms() - time < time_ms)
	{
		// check if top or bottom buttons have been pressed
		button = button_is_pressed(TOP_BUTTON | MIDDLE_BUTTON);
		if (button != 0)	// if so, turn off motors and LEDs, return button ID
		{
			set_motors(0, 0);
			return button;
		}

		lcd_goto_xy(10, 0);
		print("cur: ");
		print_unsigned_long(x2_get_motor_current(MOTOR1));
		print("   ");
		lcd_goto_xy(10, 1);
		print("cur: ");
		print_unsigned_long(x2_get_motor_current(MOTOR2));
		print("   ");
	}

	return 0;
}


// *** triggered by middle button ***
// This function tests the motors by first ramping motor1 speed from zero
// to full speed "forward", to full speed "reverse", and finally back to zero.
// It then does the same for motor2 before repeating all over again.
// While motor1 is running, the red user LED is on, otherwise it is off.
// While the currently active motor is moving "forward", the green user LED
// is on, otherwise it is off.  The LCD gives you feedback as to which motor
// is currently moving in which direction (FWD = "forward", RVS = "reverse", and
// OFF = inactive).
unsigned char motorTest()
{
	unsigned char button;

	red_led(1);
	green_led(1);

	clear();			// clear the LCD, go to the start of the first LCD line
	print("motor2");	// print to the first line of the LCD
	lcd_goto_xy(0, 1);	// go to the start of the second LCD line
	print("motor1");	// print to the second line of the LCD

	x2_set_acceleration(MOTOR1, 70, 0);
	x2_set_brake_duration(MOTOR1, 0, 0);
	x2_set_acceleration(MOTOR2, 70, 0);
	x2_set_brake_duration(MOTOR2, 0, 0);

	while (1)
	{
		lcd_goto_xy(0, 0);
		print("M1: FWD");
		x2_set_motor(MOTOR1, ACCEL_DRIVE, 255);
		button = motor_wait(700);
		if (button)
			return button;
		lcd_goto_xy(0, 0);
		print("M1:    ");
		x2_set_motor(MOTOR1, IMMEDIATE_DRIVE, 0);
		button = motor_wait(150);
		if (button)
			return button;
		lcd_goto_xy(0, 0);
		print("M1: RVS");
		x2_set_motor(MOTOR1, ACCEL_DRIVE, -255);
		button = motor_wait(700);
		if (button)
			return button;

		lcd_goto_xy(0, 0);
		print("M1: OFF");
		lcd_goto_xy(0, 1);
		print("M2: FWD");
		x2_set_motor(MOTOR1, IMMEDIATE_DRIVE, 0);
		x2_set_motor(MOTOR2, ACCEL_DRIVE, 255);
		button = motor_wait(700);
		if (button)
			return button;
		lcd_goto_xy(0, 1);
		print("M2:    ");
		x2_set_motor(MOTOR2, IMMEDIATE_DRIVE, 0);
		button = motor_wait(150);
		if (button)
			return button;
		lcd_goto_xy(0, 1);
		print("M2: RVS");
		x2_set_motor(MOTOR2, ACCEL_DRIVE, -255);
		button = motor_wait(700);
		if (button)
			return button;
		lcd_goto_xy(0, 1);
		print("M2: OFF");
		x2_set_motor(MOTOR2, IMMEDIATE_DRIVE, 0);
	}
}

void test()
{
	unsigned char button;

	clear();

	delay(200);
	print("Orangutan");	// print to the top line of the LCD
	delay_ms(400);		// delay 200 ms
	lcd_goto_xy(0, 1);	// go to the start of the second LCD line
	print(" X2");	// print to the bottom line of the LCD

	red_led(1);
	delay_ms(100);
	green_led(1);
	delay_ms(100);
	red_led2(1);
	delay_ms(100);
	green_led2(1);
	delay_ms(100);
	yellow_led(1);
	delay_ms(100);

	red_led(0);
	delay_ms(100);
	green_led(0);
	delay_ms(100);
	red_led2(0);
	delay_ms(100);
	green_led2(0);
	delay_ms(100);
	yellow_led(0);
	delay_ms(100);

	clear();			// clear the LCD, move cursor to start of top line

	print("  VBAT");

	do
	{
		// Perform 10-bit analog-to-digital conversions on ADC channel 6.
		// Average ten readings and return the result, which will be one
		// third of the battery voltage when the "ADC6 = VBAT/3" solder
		// bridge is in place on the bottom of the Orangutan PCB
		int vbat = read_battery_millivolts_x2();
		lcd_goto_xy(0, 1);	// go to the start of the second LCD line
		print_long(vbat);	// display battery voltage in millivolts
		print(" mV ");		// display the units
		delay_ms(50);		// delay for 50 ms
		button = button_is_pressed(ANY_BUTTON);	// check for button press
	}
	while (button == 0);	// loop if no buttons are being pressed


	red_led(1);
	green_led(1);
	red_led2(1);
	green_led2(1);
	yellow_led(1);


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
	// Set all five user LEDs as outputs driven low.
	// Otherwise, the LEDs are driven by the LCD and can appear
	// on or partially on.
	red_led(0);
	green_led(0);
	red_led2(0);
	green_led2(0);
	yellow_led(0);

	clear();
	
	delay(10);

	// if any button is pressed, go into the old version of the test code
	if(button_is_pressed(ANY_BUTTON))
	{
		print("Simple Test");
		wait_for_button_release(button_is_pressed(ANY_BUTTON));
		test(); // activate the simpler test code
	}

	// set up the robot
	initialize();

	// This is the "main loop" - it will run forever.
	while(1)
	{
		menu_select();
	}
}
