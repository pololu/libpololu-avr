/* $projectname$ - an application for the Pololu Orangutan SVP
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
		// Update the LCD.
		clear();
		print("Batt (mV): "); 
		lcd_goto_xy(11, 0);
		print_long(read_battery_millivolts_svp());

		red_led(1);     // Turn on the red LED.
		delay_ms(200);  // Wait for 200 ms.

		red_led(0);     // Turn off the red LED.
		delay_ms(200);  // Wait for 200 ms.
	}
}
