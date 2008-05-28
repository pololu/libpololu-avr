#include <avr/io.h>

#define RED_LED		PD1
#define GREEN_LED	PD7


extern "C" void left_led(unsigned char on)
{
	OrangutanLEDs::red(on);
}

extern "C" void right_led(unsigned char on)
{
	OrangutanLEDs::green(on);
}


static void OrangutanLEDs::red(unsigned char on)
{
	DDRD |= 1 << RED_LED;
	if (on)
		PORTD |= 1 << RED_LED;
	else
		PORTD &= ~(1 << RED_LED);
}

static void OrangutanLEDs::green(unsigned char on)
{
	DDRD |= 1 << GREEN_LED;
	if (on)
		PORTD |= 1 << GREEN_LED;
	else
		PORTD &= ~(1 << GREEN_LED);
}