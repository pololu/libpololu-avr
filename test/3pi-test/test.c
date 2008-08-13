#include <pololu/3pi.h>
#include "assert.h"
#include "qtr.h"
#include "motors.h"
#include "pot.h"
#include "battery.h"
#include "pushbuttons.h"
#include "outputs.h"
#include "leds.h"
#include "lcd.h"

#include <avr/pgmspace.h>

// Introductory messages.
const char welcome[] PROGMEM = ">g32>>c32";
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot"; // \xf7 is a greek
													// pi character
const char demo_name_line1[] PROGMEM = "Test";
const char demo_name_line2[] PROGMEM = "Program";

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

void print_two_lines_delay_1s(const char *line1, const char *line2)
{
	// Play welcome music and display a message
	clear();
	print_from_program_space(line1);
	lcd_goto_xy(0,1);
	print_from_program_space(line2);
	delay_ms(1000);
}

int main()
{
	// Load bar graph characters.
	// Together with space and the solid block at 255, this makes almost
	// all possible bar heights, with two to spare.
	unsigned char i;
	for(i=0;i<6;i++)
	{
		lcd_load_custom_character(bars+i,i);
	}
	clear();

	pololu_3pi_init(1000);

	if(test_pushbutton_tries())
		goto pushbuttons;

	play_from_program_space(welcome);
	print_two_lines_delay_1s(welcome_line1,welcome_line2);
	print_two_lines_delay_1s(demo_name_line1,demo_name_line2);

	clear();

 	test_battery();
	test_qtr();
	test_motors();
	test_pot();
	test_outputs();
	test_leds();
pushbuttons:
	test_pushbuttons();

	clear();
	print("Success");
	play("O5 c16");
	
	while(1);
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
