#include <pololu/orangutan.h>
#include <avr/io.h>
#include "assert.h"

void test_outputs()
{
	clear();
	print("PD0/PD1");

	DDRD |= 3;
	
	while(!button_is_pressed(BUTTON_B))
	{
		PORTD ^= 2;
		PORTD ^= 1;

		delay_ms(500);

		PORTD ^= 2;

		delay_ms(500);
	}

	while(button_is_pressed(ALL_BUTTONS));

	DDRD &= ~(3);
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
