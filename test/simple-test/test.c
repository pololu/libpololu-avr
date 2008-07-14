#include <pololu/orangutan.h>

int main()
{
	print("Hello!");

	play("L16 ceg>c");

	while(1)
	{
		red_led(0);
		green_led(1);

		delay_ms(100);

		red_led(1);
		green_led(0);
		
		delay_ms(100);
	}

	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
