#include <pololu/orangutan.h>
#include "assert.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define TRIES 2

uint8_t EEMEM eeprom_reset_count;

char test_pushbutton_tries()
{
	cli();
	int reset_count = eeprom_read_byte(&eeprom_reset_count);
	sei();

	if(reset_count > 6)
		reset_count = 0;

	return reset_count;
}

void test_pushbuttons()
{
	char reset_count = test_pushbutton_tries();
	if(reset_count < TRIES)
	{
		play("g32");
		clear();
		print("Power ");
		print_long(TRIES-reset_count);
		reset_count ++;
		cli();
		eeprom_write_byte(&eeprom_reset_count,reset_count);
		sei();
		while(1);
	}
	else if(reset_count < TRIES*3)
	{
		play("g32");
		clear();
		print("Reset ");
		print_long(3*TRIES-reset_count);
		reset_count ++;
		cli();
		eeprom_write_byte(&eeprom_reset_count,reset_count);
		sei();
		while(1);
	} 

	cli();
	eeprom_write_byte(&eeprom_reset_count,0);
	sei();
}


// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
