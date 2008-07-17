#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"
#include "analog.h"
#include "delay.h"
#include "lcd.h"
#include "buzzer.h"
#include "leds.h"
#include "motors.h"
#include "pushbuttons.h"
#include "qtr.h"

int main()
{
	delay_ms(500); // warming up

	lcd_init_printf();

	printf("\nAssert");
	assert(1 == 1); // make sure assert works

	test_qtr();
	test_pushbuttons();
	test_buzzer();
	test_motors();
	test_lcd();
	test_leds();
	test_analog();
	test_delay();

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
