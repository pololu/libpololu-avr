#include <pololu/orangutan.h>
#include <stdio.h>	// required for printf()

/*
 * lcd3-hello-world: for the Orangutan LV, SV, SVP, X2, and 3pi robot.
 *
 * This example program is intended for use on the Orangutan LV, SV, SVP, X2,
 * and 3pi robot (any Orangutan with an LCD).  It uses printf() from stdio.h
 * and OrangutanLCD to write to the LCD.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */



int main()
{
	lcd_init_printf();	// required if we want to use printf()

	clear();			// clear the LCD
	delay_ms(200);		// wait for 200 ms

	printf("hello");	// print "hello" on the first line of the LCD
	delay_ms(200);		// wait for 200 ms
	printf("\nworld");	// print "world" on the second line (because of '\n')
	delay_ms(2000);		// wait for 2 seconds

	clear();			// clear the LCD

	unsigned char ch;
	for (ch = 'A'; ch <= 'z'; ch++)	// demonstrate LCD wrapping
	{
		printf("%c", ch);	// print a string of characters that wraps when
		delay_ms(50);		// it reaches the end of the LCD
	}
	delay_ms(2000);		// wait for 2 seconds

	clear();			// clear the LCD

	int i;
	printf("Hex Dec");
	for(i = 0; i <= 500; i += 50)	// demonstrate LCD scrolling
	{
		delay_ms(800 - i);	// the delay gets shorter as i gets bigger
		printf("\n%03X %3d", i, i);	// print i as 3-digit, zero-padded hex
									// and a space-padded 3-digit decimal
	}
	delay_ms(2000);		// wait for 2 seconds

	clear();			// clear the LCD
	printf("Trimpot:");

	while (1)			// continuously display the trimpot voltage in mV
	{
		lcd_goto_xy(0, 1);	// go to start of second LCD row
		printf("%4u mV", to_millivolts(read_trimpot()));	// print trimpot voltage
		delay_ms(50);	// wait for 50 ms to reduce LCD flicker
	}
}
