#include <pololu/orangutan.h>
#if defined(_ORANGUTAN_X2) || defined(_ORANGUTAN_SVP)
// these functions are undefined in the Pololu AVR library for the
// Orangutan SVP and Orangutan X2, so we define them here to prevent
// compile errors.
int read_temperature_f() { return 0; }
unsigned int read_battery_millivolts_sv() { return 0; }
#endif

/*
 * analog2: for the Orangutan LV, SV, SVP, X2, and 3pi robot
 *
 * This example uses the OrangutanAnalog functions to read the voltage
 * output of the trimpot (in millivolts) and to read the battery voltage
 * on the Orangutan SV, SVP, X2, or 3pi or the temperature sensor output
 * in degrees Farenheit on the Orangutan LV.  These values are
 * printed to the LCD 10 times per second.
 *
 * You should see the trimpot voltage change as you turn it, and you can
 * get the temperature reading to slowly increase by holding a finger on the
 * underside of the Orangutan LV's PCB near the center of the board.
 * Be careful not to zap the board with electrostatic discharge if you
 * try this!
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
	unsigned char button;
	int temp;

	clear();			// clear LCD

#ifdef _ORANGUTAN_SVP			// if running on an Orangutan SVP

	print("Orangutan SVP");
	button = BUTTON_C;

#elif defined(_ORANGUTAN_X2)	// if running on an Orangutan X2

	print("Orangutan X2");
	button = BUTTON_C;

#else		// if running on something else (LV, SV, 3pi, Baby Orangutan)

	print("select");
	lcd_goto_xy(0, 1);	// go to start of second LCD row
	print("device");
	delay_ms(1000);

	clear();
	print("A  B  C");
	lcd_goto_xy(0, 1);
	print("LV SV 3p");

	// use button press to determine which device is being used
	button = wait_for_button_press(ANY_BUTTON);
	
	clear();
	switch (button)
	{
		case BUTTON_A:
			print(" Orang.");
			lcd_goto_xy(0, 1);
			print("  LV");
			break;
		case BUTTON_B:
			print(" Orang.");
			lcd_goto_xy(0, 1);
			print("  SV");
			break;
		case BUTTON_C:
			print("  3pi");
			lcd_goto_xy(0, 1);
			print(" Robot");
			break;
	}

#endif

	delay(1000);
	clear();

	set_analog_mode(MODE_10_BIT);	// 10-bit analog-to-digital conversions


	while(1)						// loop forever
	{
		lcd_goto_xy(0, 0);			// LCD cursor to home position (upper-left)
		print_long(to_millivolts(read_trimpot()));  // trimpot output in mV
		print(" mV   ");			// added spaces are to overwrite left over chars

		lcd_goto_xy(0, 1);			// LCD cursor to start of the second line

		switch (button)
		{
			case BUTTON_A:	// using Orangutan LV, so display temp sensor data
				temp = read_temperature_f();	// get temp in tenths of a degree F
				print_long(temp/10);		// get the whole number of degrees
				print_character('.');		// print the decimal point
				print_long(temp%10);		// print the tenths digit
				print_character(223);		// print a degree symbol
				print("F   ");				// added spaces are to overwrite left over chars
				break;

			case BUTTON_B:	// using Orangutan SV, so display battery voltage
				// on SV, analog channel 6 (ADC6) is connected to 1/3 the system voltage
				// through a voltage divider.  We average 10 readings on channel 6, convert
				// to millivolts, and then multiply by 3 to get the battery voltage
				print_long(to_millivolts(analog_read_average(6, 10)) * 3);
				// another option would be to use the line:
				// print_long(read_battery_millivolts_sv());
				print(" mV   ");
				break;

			case BUTTON_C:	// using Orangutan SVP or X2 or 3pi robot
				print_long(read_battery_millivolts());
				print(" mV   ");
				break;
		}

		delay_ms(50);				// wait for 50 ms to decrease LCD flicker
	} 
}