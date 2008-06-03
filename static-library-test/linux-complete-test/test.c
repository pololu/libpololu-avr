#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"
#include "analog.h"
#include "delay.h"

int main()
{
	delay_ms(500); // warming up

	lcd_init_printf();

	printf("\nAssert");
	assert(1 == 1); // make sure assert works
	test_analog();
	test_delay();

	clear();
	printf("\nSuccess");
	play("O5 c16");
	
	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
