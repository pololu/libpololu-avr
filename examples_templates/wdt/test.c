#include <pololu/orangutan.h>
#include <avr/wdt.h>	// provides functions for using the watchdog timer

/*
 * wdt: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This program demonstrates the use of the watchdog timer.  It routinely resets
 * the watchdog timer (which is set to a period of 4 seconds) unless a button
 * is held down.  The LCD displays a countdown timer until the watchdog timer
 * reaches its limit and resets the device.  The program also displays what
 * was responsible for the board's last reset: external reset, brownout,
 * power-on, or watchdog timer.  Most Orangutans have brownout detection set to
 * 4.3V by default, so powering them down results in a brownout reset, not a
 * power-on reset.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */


int main()
{
	// when working with the watchdog timer, it is important that your
	// program's first action is to clear the MCUSR register
	// (this is accomplished by calling clear_reset_flags()) and disable
	// the watchdog timer.  If the watchdog timer ever resets your program,
	// the watchdog timer defaults to enabled with the shortest possible
	// period (15 ms), which can lead to repeated resets if your program
	// does not initially clear MCUSR and disable the watchdog timer.
	unsigned char rf = get_reset_flags();
	clear_reset_flags();
	wdt_disable();

	clear();
	print("boot");
	unsigned char i;
	for (i = 0; i < 4; i++)
	{
		delay_ms(150);
		print(".");
	}
	delay_ms(300);

	wdt_enable(WDTO_2S);	// enable watchdog timer with 2 sec period


	while (1)
	{
		clear();			// clear LCD

		// display last reset source on the LCD
		if (rf & WATCHDOG_RESET)
			print("WATCHDOG");
		else if (rf & BROWNOUT_RESET)
			print("BROWNOUT");
		else if (rf & EXTERNAL_RESET)
			print("EXTERNAL");
		else if (rf & POWERON_RESET)
			print("POWER-ON ");
		else
			print("none");

		while (1)
		{
			lcd_goto_xy(0, 1);	// go to start of second LCD row
			print(" BUTTON ");

			wdt_reset();	// reset the watchdog timer

			unsigned long time = get_ms();

			while (button_is_pressed(ANY_BUTTON))
			{
				// We loop here while button is held down and DO NOT
				// reset the watchdog timer.
				// Display a four-second countdown until WDT resets us.
				int wdt_ms = 2000 - (int)(get_ms() - time);
				lcd_goto_xy(0, 1);	// go to start of second LCD row
				print("WD ");
				if (wdt_ms < 0)
					print("0.00s");
				else
				{
					print_long(wdt_ms / 1000);		// seconds
					print(".");
					print_long((wdt_ms/100)%10);	// tenths of seconds
					print_long((wdt_ms/10)%10);		// hundredths of seconds
					print("s");
				}
			}
		}
	}
}