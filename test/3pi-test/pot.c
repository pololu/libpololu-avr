#include <pololu/3pi.h>
#include "assert.h"

void test_pot()
{
	clear();

	while(!button_is_pressed(BUTTON_B))
	{
		long start = get_ms();
		char elapsed_ms;
		int value;

		set_analog_mode(MODE_10_BIT);

		lcd_goto_xy(0,0);
		print("Trimpot");
		lcd_goto_xy(0,1);
		print_long(read_trimpot());
		print("   "); // to clear the display

		while((elapsed_ms = get_ms() - start) < 100)
		{
			value = read_trimpot();
			play_frequency(value, 200, 15);
		
			if(value < elapsed_ms*10)
			{
				red_led(0);
				green_led(1);
			}
			else
			{
				red_led(1);
				green_led(0);
			}
		}
	}

	while(button_is_pressed(BUTTON_B));
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
