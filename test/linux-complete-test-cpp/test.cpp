#include <pololu/orangutan>
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

static OrangutanLCD lcd;
static OrangutanBuzzer buzzer;

int main()
{
	OrangutanBuzzer::play(">>a16");
	OrangutanDelay::ms(500); // warming up

	lcd.initPrintf();

	lcd.print("Assert");
	assert(1 == 1); // make sure assert works

	test_qtr();
	test_analog();
	test_pushbuttons();
	test_buzzer();
	test_motors();
	test_lcd();
	test_leds();
	test_delay();

	lcd.clear();
	lcd.print("Success");
	
	buzzer.play("O5 c16");

	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
