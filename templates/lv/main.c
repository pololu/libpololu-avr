/* $projectname$ - an application for the Pololu Orangutan LV
 *
 * Author:  $username$
 * Created: $time$
 */

#include <pololu/orangutan.h>

int main()
{
	play_from_program_space(PSTR(">g32>>c32"));  // Play welcoming notes.

	while(1)
	{
		// Print temperature (in degrees C) on the LCD.
		clear();
		print_long(read_temperature_c()/10);

		red_led(1);     // Turn on the red LED.
		delay_ms(200);  // Wait for 200 ms.

		red_led(0);     // Turn off the red LED.
		delay_ms(200);  // Wait for 200 ms.
	}
}
