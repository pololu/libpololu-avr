#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"
#include "analog.h"

int main()
{
	lcd_init_printf();

	printf("\nAssert");
	assert(1 == 1); // make sure assert works
	test_analog();

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
