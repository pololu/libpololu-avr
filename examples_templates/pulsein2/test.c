#include <pololu/orangutan.h>

/*
 * pulsein2: for the Orangutan LV, SV, SVP, Baby Orangutan, and 3pi robot.
 *
 * This program uses the OrangutanPulseIn functions to measure the frequency and
 * duty cycle of the PWM supplied to the buzzer as we play a melody.  The
 * frequency is displayed on the top line of the LCD in Hz and the duty cycle is
 * displayed as a percentage on the bottom line of the LCD.
 * 
 * Note: To get this program to display something interesting on while running on the
 * Orangutan X2, you will need to connect pin PD4 to the ungrounded buzzer pin using
 * a wire, since the buzzer on the Orangutan X2 is controlled by an auxiliary MCU
 * rather than the main MCU.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

// This array of pins is used to initialize the OrangutanPulseIn routines.  To measure
// pulses on multiple pins, add more elements to the array.  For example:
// const unsigned char pulseInPins[] = { IO_D0, IO_C0, IO_C1 };
// BUZZER_IO is IO_D4 on the Orangutan SVP and IO_B2 on the LV, SV, Baby, and 3pi
const unsigned char pulseInPins[] = { BUZZER_IO };


int main()
{
	clear();  // clear LCD

	pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0

	// We will play Mary Had A Little Lamb and use the OrangutanPulseIn library to measure the frequency
	// and duty cycle of the PWM signal supplied to the buzzer:
	play("!L4 MS EDCDEEE2 DDD2 EGG2 V9 EDCDEEEE DDEDC1 R T240 O6 V14 EDCDEEE2 DDD2 EGG2 EDCDEEEE DDEDC1");

	while (1)  // main loop
	{
		unsigned long curPulse;		// length of current pulse in ticks (0.4 us)
		unsigned char state;		// current state of input (1 if high, 0 if low)
		// get current pulse state for D0
		get_current_pulse_state(0, &curPulse, &state);	// pass arguments as pointers

		// if more than 300 ms have elapsed since the last pin
		// change on this channel, we indicate that pulses have stopped
		if (pulse_to_microseconds(curPulse) >= 300000UL)
		{
			if (state == HIGH)		// if line is currently high
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("Pin HIGH  ");
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("          ");	// clear the row by overwriting with spaces
			}
			else
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("          ");	// clear the row by overwriting with spaces
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("Pin  LOW  ");
			}
		}
		else if (new_high_pulse(0) && new_low_pulse(0))	// if we have new high and low pulses
		{
			unsigned long high_pulse = get_last_high_pulse(0);
			unsigned long period_in_ticks = high_pulse + get_last_low_pulse(0);

			// compute frequency as 1 / period = 1 / (0.4us * period_in_ticks)
			//  = 2.5 MHz / period_in_ticks
			unsigned long frequency_in_hz = 2500000UL / period_in_ticks;

			// duty cycle = high pulse / (high pulse + low pulse)
			// we multiply by 100 to convert it into a percentage and we add half of the denominator to
			// the numerator to get a properly rounded result
			unsigned long duty_cycle_percent = (100 * high_pulse + period_in_ticks/2) / period_in_ticks;

			lcd_goto_xy(0, 0);		// go to start of first LCD row
			print_unsigned_long(frequency_in_hz);		// print the measured PWM frequency
			print(" Hz      ");
			lcd_goto_xy(0, 1);		// go to start of second LCD row
			print("DC: ");
			print_unsigned_long(duty_cycle_percent);	// print the measured PWM duty cycle
			print("%  ");
		}
	}
}
