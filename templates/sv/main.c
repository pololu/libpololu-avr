/* $projectname$ - an application for the Pololu Orangutan SV
 *
 * Created: $time$
 *  Author: $username$
 */

#include <pololu/orangutan.h>

int main()
{
	play_from_program_space(PSTR(">g32>>c32"));  // Play welcoming notes.

	while(1)
	{
		// Print battery voltage (in mV) on LCD.
		clear();
		print_long(read_battery_millivolts_sv());

		red_led(1);     // Turn on the red LED.
		delay_ms(200);  // Wait for 200 ms.

		red_led(0);     // Turn off the red LED.
		delay_ms(200);  // Wait for 200 ms.
	}
}
