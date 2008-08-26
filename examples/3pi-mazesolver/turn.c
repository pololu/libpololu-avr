/*
 * Code to perform various types of turns.  The delays here had to be
 * calibrated for the 3pi's motors.
 */

#include <pololu/3pi.h>

void turn_left()
{
	set_motors(0,0);
	delay_ms(100);

	set_motors(-80,80);
	delay_ms(200);

	set_motors(0,0);
	delay_ms(100);
}

void turn_right()
{
	set_motors(0,0);
	delay_ms(100);

	set_motors(80,-80);
	delay_ms(200);

	set_motors(0,0);
	delay_ms(100);
}

void turn_around()
{
	set_motors(0,0);
	delay_ms(100);

	set_motors(80,-80);
	delay_ms(400);

	set_motors(0,0);
	delay_ms(100);
}

