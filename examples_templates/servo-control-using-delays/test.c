/*
 * servo-control-using-delays - demo code for controlling servos with the
 * Pololu Baby Orangutan B and Orangutan SV-128/328 Robot Controllers.
 * This example uses some I/O lines that are not available on the
 * Orangutan X2 and Orangutan SVP, so it will not work properly on those
 * devices without some modification.  Similar code could also be used to
 * control one or two servos on the 3pi robot.
 * 
 * Uses 8 ports: PD0, PD1, and PC0-5.
 *
 * On the Orangutan SV, if the 5V jumper option is selected, the servos
 * can be connected directly to the three-pin port headers, since the
 * regulator is able to supply 3 A of current.  Note that the on-board
 * voltage regulators on Orangutans other than the SV and SVP do not
 * have enough power to drive servos, so you will need to power servos
 * from a separate source when using the Orangutan LV, Baby Orangutan,
 * Orangutan X2, or 3pi robot.
 *
 * The structure here is the easiest way to control servos, but it is
 * complicated to do other things at the same time with this method.
 * For more complicated programs, you will need to replace the
 * delay_us calls with routines that use the timers on the AVR, either
 * with interrupts or with some other method.  The OrangutanServos
 * library takes care of interrupt-driven servo-pulse generation for
 * you.
 *
 * Note that this example was written before the OrangutanDigital
 * library existed, so it performs digital I/O using direct manipulation
 * of the AVR's digital I/O registers.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 */

#include <pololu/orangutan.h>

// we need io.h for the port definitions
#include <avr/io.h>

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
	DDRD |= 1 << DD0;
	DDRD |= 1 << DD1;
	DDRC |= 1 << DD0;
	DDRC |= 1 << DD1;
	DDRC |= 1 << DD2;
	DDRC |= 1 << DD3;
	DDRC |= 1 << DD4;
	DDRC |= 1 << DD5;

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

		/* for an Orangutan with LCD, uncomment this code to display
		   the position of servo 0: */
		//		clear();
		//		print_long(s[0]);

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
