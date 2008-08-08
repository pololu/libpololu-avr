#include <pololu/3pi.h>
#include <stdio.h>
#include "assert.h"
#include "qtr.h"

int main()
{
	pololu_3pi_init(1000);
	lcd_init_printf();

	printf("\nAssert");
	assert(1 == 1); // make sure assert works

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
