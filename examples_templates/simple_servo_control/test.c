/*
 * simple_servo_program - demo code for controlling servos with the
 * Pololu SV-128/328 Robot Controllers.  If the 5 V jumper option is
 * selected, the servos can be connected directly to the three-pin
 * port headers, since the SV-xx8 is able to supply 3 A of current
 * directly from its 5 V regulator.  Similar code could also be used
 * to control one or two servos on the 3pi robot.
 *
 * The structure here is the easiest way to control servos
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

#include <pololu/orangutan.h>

// we need io.h for the port definitions
#include <avr/io.h>

// compatibility with the mega328p requires these definitions
#include <pololu/atmega328p_pins.h>

/*
 * This function adds a given amount to the integer pointed to by s.
 */
void inc(int *s, char increment)
{
    (*s)+=increment;
    if(*s >= 1800)
		*s = 1200;
}

int main()
{
	// set all of the ports to outputs
	DDRD |= 1 << PD0;
	DDRD |= 1 << PD1;
	DDRC |= 1 << PC0;
	DDRC |= 1 << PC1;
	DDRC |= 1 << PC2;
	DDRC |= 1 << PC3;
	DDRC |= 1 << PC4;
	DDRC |= 1 << PC5;

	// the positions of the servos, which are given by an integer
	// from 1200 to 1800 representing a delay in us
	int s[8];

	// the amount to increment the servos by every 20 ms, to make
	// interesting motion
	char increment[8] = {9,10,11,12,13,14,15,16};

	// initialize all servos to 1200 us
	int i;
	for(i=0;i<8;i++)
		s[i] = 1200;

	while(1)
	{
		// The body of this loop should take 20 ms to execute, since
		// servos are supposed to be updated at 50 Hz.  We spend 2 ms
		// on each of the 8 servos.

		for(i=0;i<8;i++)
		{
			// This is the 2 ms block for servo i.

			// set the pin high
			if(i < 2)
				PORTD |= 1 << i;
			else
				PORTC |= 1 << (i-2);

			// delay for the ON time, 1200 to 1800 us
			delay_us(s[i]);

			// set the pin low
			if(i < 2)
				PORTD &= ~(1 << i);
			else
				PORTC &= ~(1 << (i-2));

			// delay for the rest of the 2ms
			delay_us(2000 - s[i]);

			// increment the position counter to make the servos move
			// in an interesting pattern
			inc(&s[i],increment[i]);
		}

		// display the position of servo 0 on the LCD
		clear();
		print_long(s[0]);

		// The FOR loop above took 16 ms, so we need 4 more ms to
		// complete the 20 ms period.
		delay_ms(4);
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
