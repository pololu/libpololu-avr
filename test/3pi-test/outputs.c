#include <pololu/orangutan.h>
#include <avr/io.h>
#include "assert.h"

void test_outputs()
{
	int i;

	clear();
	print("PD0/PD1");

	DDRD |= 3;
	
	while(!button_is_pressed(BUTTON_B))
	{
		PORTD ^= 2;
		PORTD ^= 1;

		for(i=0;i<50 && !button_is_pressed(BUTTON_B);i++)
			delay_ms(10);

		PORTD ^= 2;

		for(i=0;i<50 && !button_is_pressed(BUTTON_B);i++)
			delay_ms(10);
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
