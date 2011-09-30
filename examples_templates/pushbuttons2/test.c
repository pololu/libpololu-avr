#include <pololu/orangutan.h>
#include <stdio.h>	// for printf()

/*
 * pushbuttons2: for the Orangutan LV, SV, SVP, X2, and 3pi robot.
 *
 * This example uses the OrangutanPushbuttons library to detect user input
 * from the pushbuttons.  Each button press and release triggers an event
 * (the buzzer plays a note) while the LCD continuously displays the time
 * that has elapsed.  The functions get_single_debounced_button_press()
 * and get_single_debounced_button_release() make it easy to perform such
 * button-triggered activities within your main loop.  These functions
 * are non-blocking, so the rest of your main loop can run while waiting
 * for the buttons to be pressed or released, and they should be called
 * repeatedly.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
	lcd_init_printf();
	clear();	// clear the LCD
	printf("Time: ");
	
	while (1)
	{
		unsigned char button = get_single_debounced_button_press(ANY_BUTTON);
		switch (button)
		{
			case BUTTON_A:
				play_note(A(4), 50, 10);
				break;
			case BUTTON_B:
				play_note(B(4), 50, 10);
				break;
			case BUTTON_C:
				play_note(C(5), 50, 10);
		}

		button = get_single_debounced_button_release(ANY_BUTTON);
		switch (button)
		{
			case BUTTON_A:
				play_note(A(5), 50, 10);
				break;
			case BUTTON_B:
				play_note(B(5), 50, 10);
				break;
			case BUTTON_C:
				play_note(C(6), 50, 10);
		}

		unsigned long ms = get_ms();	// get elapsed milliseconds
		// convert to the current time in minutes, seconds, and hundredths of seconds
		unsigned char centiseconds = (ms / 10) % 100;
		unsigned char seconds = (ms / 1000) % 60;
		unsigned char minutes = (ms / 60000) % 60;

		lcd_goto_xy(0, 1);				// go to the start of the second LCD row
		// print as [m]m:ss.cc (m = minute, s = second, c = hundredth of second)
		printf("%2u:%02u.%02u", minutes, seconds, centiseconds);
	}

}
