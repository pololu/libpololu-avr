#include <pololu/orangutan.h>
#include <stdio.h>
#include "assert.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define TRIES 5

uint8_t EEMEM eeprom_reset_count;

char test_pushbutton_tries()
{
	cli();
	int reset_count = eeprom_read_byte(&eeprom_reset_count);
	sei();

	if(reset_count > 10)
		reset_count = 0;

	return reset_count;
}

void test_pushbuttons()
{
	char i;

	char reset_count = test_pushbutton_tries();
	if(reset_count < TRIES)
	{
		play("g32");
		printf("\nPress\nReset %d",TRIES-reset_count);
		reset_count ++;
		cli();
		eeprom_write_byte(&eeprom_reset_count,reset_count);
		sei();
		while(1);
	}
	else if(reset_count < TRIES*2)
	{
		play("g32");
		printf("\nPress\nPower %d",2*TRIES-reset_count);
		reset_count ++;
		cli();
		eeprom_write_byte(&eeprom_reset_count,reset_count);
		sei();
		while(1);
	} 

	cli();
	eeprom_write_byte(&eeprom_reset_count,0);
	sei();

	for(i=0;i<TRIES;i++)
	{
		clear();
		printf("\nPress A\n%d",TRIES-i);
		unsigned char button = wait_for_button(BUTTON_A);
		printf(" %d",button);
		assert(button == BUTTON_A);
		play("g32");
	}

	for(i=0;i<TRIES;i++)
	{
		clear();
		printf("\nPress B\n%d",TRIES-i);
		unsigned char button = wait_for_button(BUTTON_B);
		printf(" %d",button);
		assert(button == BUTTON_B);
		play("g32");
	}

	for(i=0;i<TRIES;i++)
	{
		clear();
		printf("\nPress C\n%d",TRIES-i);
		unsigned char button = wait_for_button(BUTTON_C);
		printf(" %d",button);
		assert(button == BUTTON_C);
		play("g32");
	}

}


// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
