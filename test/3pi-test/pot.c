#include <pololu/3pi.h>
#include "assert.h"

void test_pot()
{
	clear();

	while(!button_is_pressed(BUTTON_A))
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
		lcd_goto_xy(6,1);
		print("A");

		while((elapsed_ms = get_ms() - start) < 100)
		{
			value = read_trimpot();
			play_frequency(value, 200, 15);
		
			if(value < elapsed_ms*10)
			{
				red_led(1);
				green_led(0);
			}
			else
			{
				red_led(0);
				green_led(1);
			}
		}
	}

	while(button_is_pressed(BUTTON_A));
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
