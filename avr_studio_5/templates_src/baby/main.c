/* $projectname$ - an application for the Pololu Baby Orangutan B
 *
 * Created: $time$
 *  Author: $username$
 */

#include <pololu/orangutan.h>

int main()
{
	while(1)
	{
		red_led(1);     // Turn on the red LED.
		delay_ms(200);  // Wait for 200 ms.

		red_led(0);     // Turn off the red LED.
		delay_ms(200);  // Wait for 200 ms.
	}
}
