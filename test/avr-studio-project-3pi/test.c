#include <pololu/3pi.h>

void display_readings(const unsigned int *calibrated_values)
{
	unsigned char i;

	for(i=0;i<5;i++) {
		const char display_characters[10] = {' ','.',',','-','+','o','O','0','@','#'};
		// values from 0 to 9
		char c = display_characters[calibrated_values[i]/101];
		print_character(c);
	}
}

int main()
{
	unsigned int counter;

	pololu_3pi_init(2000);

	// display temperature and wait for button press
  
	while(!button_is_pressed(BUTTON_B))
	{
		int bat = battery_millivolts();

		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0,1);
		print("Press B");

		delay_ms(50);
	}

	wait_for_button_release(BUTTON_B);
	delay_ms(500);

	// auto-calibration

	for(counter=0;counter<80;counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(40,-40);
		else
			set_motors(-40,40);

		clear();
		lcd_goto_xy(0,1);
		print("Press B");

		calibrate_line_sensors();

		delay_ms(20);
	}
	set_motors(0,0);

	// display calibrated values

	while(!button_is_pressed(BUTTON_B))
	{
		unsigned int sensors[5] = {1,2,3,4,5};
		unsigned int position = read_line(sensors);

		clear();
		print_long(position);
		lcd_goto_xy(0,1);
		display_readings(sensors);

		delay_ms(50);
	}
	wait_for_button_release(BUTTON_B);

	clear();

	play("L16 cdegreg4");

	print("Go!");		

	while(is_playing());

	while(1)
	{
		unsigned int sensors[5] = {1,2,3,4,5};
		unsigned int position = read_line(sensors);

		if(position < 2000)
			set_motors(0,100);
		else
			set_motors(100,0);
	}

	return 0;
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
