#include <pololu/3pi.h>
#include <stdio.h>
#include "assert.h"
#include "qtr.h"
#include "motors.h"

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
	lcd_init_printf();

	printf("\nAssert");
	assert(1 == 1); // make sure assert works

	test_motors();
	test_qtr();

	clear();
	printf("\nSuccess");
	play("O5 c16");
	
	while(1);
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
