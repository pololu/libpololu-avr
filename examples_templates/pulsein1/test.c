#include <pololu/orangutan.h>

/*
 * pulsein1: for the Orangutan LV, SV, SVP, X2, Baby Orangutan, and 3pi robot.
 *
 * This program uses the OrangutanPulseIn functions to measure hobby servo
 * pulses connected to pin D0.  It also generates hobby servo pulses on pin D1,
 * so if you do not have a external pulse source (such as an RC receiver or a
 * servo controller), you can use a wire to connect D1 to D0 and see the PulseIn
 * library measure the pulses put out by D1 and display the high and low durations
 * on the LCD.  The servo pulses are generated using the OrangutanServo library
 * and the length of the servo pulses is controlled by the position of the
 * user trimpot.
 *
 * Note: if you do not have a jumper wire handy, you can redefine the global
 * pulseInPins to be:
 * const unsigned char pulseInPins[] = { IO_D1 };
 * so that you are directly monitoring the pulses being generated on pin D1.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

// This array of pins is used to initialize the OrangutanPulseIn routines.  To measure
// pulses on multiple pins, add more elements to the array.  For example:
// const unsigned char pulseInPins[] = { IO_D0, IO_C0, IO_C1 };
const unsigned char pulseInPins[] = { IO_D0 };

// This array of pins is used to initialize the OrangutanServo routines.  To generate
// servo pulses on multiple pins, add more elements to the array.  For example:
// const unsigned char servoPinsp[] = { IO_D1, IO_C0, IO_C1 };
const unsigned char servoPins[] = { IO_D1 };


int main()
{
	// we use the "B" versions of the servo routines so it will work the same way
	// on all Orangutans.  If you are not using the Orangutan SVP, you can replace
	// the following line with:
	// servos_start(servoPins, 1);
	// and the function set_servo_target_b() with set_servo_target()
	servos_start_extended(0, 0, servoPins, 1);

	// pulse_in_start() will configure all of the supplied pulseInPins as inputs,
	// but if we want the internal pull-up enabled, we need to do it ourselves:
	set_digital_input(IO_D0, PULL_UP_ENABLED);	// enable internal pull-up on PD0

	clear();  // clear LCD

	pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0
	set_analog_mode(MODE_10_BIT);		// analog results range from 0 to 1023
	unsigned long time = 0;

	while (1)  // main loop
	{
		// generate our servo pulse output on PD1 based on the position of the trimpot.
		// update the position at most once every 50 ms
		if (get_ms() - time >= 50)
		{
			set_servo_target_b(0, 1250 + read_trimpot() / 2);
			time = get_ms();
		}

		struct PulseInputStruct pulse_info;
		get_pulse_info(0, &pulse_info);  // get pulse info for D0

		// if more than 250000 ticks (100 ms) have elapsed since the last pin
		// change on this channel, we indicate that pulses have stopped
		if (get_ticks() - pulse_info.lastPCTime >= 250000UL)
		{
			if (pulse_info.inputState == HIGH)	// if line is currently high
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("D0: HIGH  ");
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("          ");	// clear the row by overwriting with spaces
			}
			else
			{
				lcd_goto_xy(0, 0);	// go to start of first LCD row
				print("          ");	// clear the row by overwriting with spaces
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("D0:  LOW  ");
			}
		}
		else if (pulse_info.newPulse == HIGH_PULSE)
		{
			lcd_goto_xy(0, 0);		// go to start of first LCD row
			// print the length (in us) of the most recently received high pulse
			print_unsigned_long(pulse_to_microseconds(pulse_info.lastHighPulse));
			print(" us      ");
			lcd_goto_xy(0, 1);		// go to start of second LCD row
			// print the length (in us) of the most recently received low pulse
			print_unsigned_long(pulse_to_microseconds(pulse_info.lastLowPulse));
			print(" us      ");
		}
	}
}
