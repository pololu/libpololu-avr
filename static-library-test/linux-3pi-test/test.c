#include <pololu/3pi.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>


void display_readings(const unsigned int *calibrated_values)
{
	unsigned char i;

	for(i=0;i<5;i++) {
		const char display_characters[10] = {' ',0,0,1,2,3,4,5,6,255};
		// values from 0 to 10
		char c = display_characters[calibrated_values[i]/101];
		print_character(c);
	}
}

const prog_char welcome[] PROGMEM = ">g32>>c32";

int main()
{
	//int milli_minute = 1000*60;
	int resolution = 60000;  //15 seconds
	unsigned int* address = 0;

	pololu_3pi_init(2000);
	delay_ms(1000);
	print("BattTest");
	lcd_goto_xy(0,1);
	print("Press B");
	wait_for_button_press(BUTTON_B);

	set_motors(255,255);
	while((int)address < 512)
	{
		int bat = battery_millivolts();
		eeprom_write_word(address,(int)bat);
		lcd_goto_xy(0,1);
		clear();
		print_long(bat);
		address++;
		delay_ms(resolution);
	}
	set_motors(0,0);
	print("MEM END");
	eeprom_write_word((unsigned int*)0,(int)22);
	while(1);
	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
