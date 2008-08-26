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

// Turns according to the parameter dir, which should be 'L', 'R', 'S'
// (straight), or 'B' (back).
void turn(unsigned char dir)
{
	switch(dir)
	{
	case 'L':
		turn_left();
		break;
	case 'R':
		turn_right();
		break;
	case 'S':
		// Don't do anything!
		break;
	case 'B':
		turn_around();
		break;
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
