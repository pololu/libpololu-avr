#include <pololu/3pi.h>
#include "assert.h"

void test_battery()
{
	clear();

	while(!button_is_pressed(BUTTON_A))
	{
		int bat = read_battery_millivolts();

		lcd_goto_xy(0,0);
		print_long(bat);
		print(" mV");
		lcd_goto_xy(0,1);
		print("Press A");

		delay_ms(100);
	}

	while(button_is_pressed(ALL_BUTTONS));
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
