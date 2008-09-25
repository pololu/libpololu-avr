#define F_CPU 20000000
#include <avr/io.h>
#include <avr/delay.h>

int main()
{    
	DDRD |= 1<<PD1;
	PORTD |= 1<<PD1;
	DDRD |= 1<<PD7;
	PORTD |= 1<<PD7;

	int i;

	DDRB |= 1<<PB2;
	for(i=0;i<150;i++)
	{
		PORTB ^= 1<<PB2;
		_delay_ms(1);
	}
	for(i=0;i<200;i++)
	{
		PORTB ^= 1<<PB2;
		_delay_us(750);
	}
	DDRB &= ~(1<<PB2);

	while(1)
	{		
		PORTD |= 1<<PD1;
		PORTD &= ~(1<<PD7);
		_delay_ms(500);
			
		PORTD |= 1<<PD7;
		PORTD &= ~(1<<PD1);
		_delay_ms(500);
	}	
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
