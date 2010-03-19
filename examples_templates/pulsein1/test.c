#include <pololu/orangutan.h>

/*
 * pulsein1: for for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This program uses the OrangutanPulseIn functions to control measure hobby servo
 * pulses connected to pin D0.  It also generates hobby servo pulses on pin D1,
 * so if you do not have a external pulse source (such as an RC receiver or a
 * servo controller), you can use a wire to connect D1 to D0 and see the PulseIn
 * library measure the pulses put out by D1 and display the high and low durations
 * on the LCD.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

// This array of pins is used to initialize the PulseIn routines.  To measure
// pulses on multiple pins, add more elements to the array.  For example:
// const unsigned char pulseInPins[] = { IO_D0, IO_C0, IO_C1 };
const unsigned char pulseInPins[] = { IO_D0 };


int main()
{
	unsigned long time_ms = get_ms();
	unsigned int high_pulse_us = 1000;
	char upcounting = 1;

	set_digital_output(IO_D1, LOW);

	clear();  // clear LCD

	// using the parameter MAX_PULSE_26MS means we can only measure pulses
	// up to 26 ms long, but gives 0.4 us resolution.  This is a good setting
	// for hobby servo pulses, which are usually 50 Hz signals (20 ms period).
	// Other options are MAX_PULSE_3MS, _200MS, _800MS, and _3000MS.
	pulse_in_init(pulseInPins, 1, MAX_PULSE_26MS);

	while (1)  // main loop
	{
		pulse_in_update();  // this must be called at least once every 26 ms
							// when using MAX_PULSE_26MS

		struct PulseInputStruct pi = get_pulse_info(0);  // get pulse info for D0

		if (pi.curPulseWidth == MAX_PULSE)
		{
			// input line has been in the same state for longer than we can time 
			// (e.g. it is held high or low)
			if (pi.inputState == HIGH)
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("D0: HIGH");
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("        ");	// clear the row by overwriting with spaces
			}
			else
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("        ");	// clear the row by overwriting with spaces
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("D0:  LOW");
			}
		}
		else if (pi.newPulse == HIGH_PULSE)
		{
			lcd_goto_xy(0, 0);		// go to start of first LCD row
			// print the length (in us) of the most recently received high pulse
			if (pi.lastHighPulse != MAX_PULSE)
				print_unsigned_long(pulse_to_microseconds(pi.lastHighPulse));
			print(" us    ");
			lcd_goto_xy(0, 1);		// go to start of second LCD row
			// print the length (in us) of the most recently received high pulse
			if (pi.lastLowPulse != MAX_PULSE)
				print_unsigned_long(pulse_to_microseconds(pi.lastLowPulse));
			print(" us    ");
		}

		// generate pulse output on pin D1
		if (get_ms() - time_ms >= 20)
		{
			time_ms = get_ms();
			set_digital_output(IO_D1, HIGH);
			delay_us(high_pulse_us);
			set_digital_output(IO_D1, LOW);

			if (upcounting)
				high_pulse_us += 10;
			else
				high_pulse_us -= 10;

			if (high_pulse_us > 2000)
			{
				high_pulse_us = 2000;
				upcounting = 0;
			}
			if (high_pulse_us < 1000)
			{
				high_pulse_us = 1000;
				upcounting = 1;
			}
		}

	}
}
