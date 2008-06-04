#include <pololu/Orangutan.h>
#include <stdio.h>
#include "assert.h"
#include "analog.h"
#include "delay.h"
#include "lcd.h"
#include "buzzer.h"
#include "leds.h"
#include "motors.h"
#include "pushbuttons.h"

static OrangutanLCD lcd;
static OrangutanBuzzer buzzer;

int main()
{
	OrangutanDelay::ms(500); // warming up

	lcd.print("\nAssert");
	assert(1 == 1); // make sure assert works

	test_pushbuttons();
	//	test_motors();
	//	test_analog();
	//	test_delay();
	//	test_lcd();
	//	test_leds();
	//	test_buzzer();

	lcd.clear();
	lcd.print("\nSuccess");
	
	buzzer.play("O5 c16");
	
	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
