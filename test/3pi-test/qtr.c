#include <pololu/3pi.h>
#include "assert.h"
#include "test.h"

void display_values(unsigned int *values, unsigned int max)
{
	unsigned char i;

	unsigned char characters[] = {' ',0,1,2,3,4,5,255};

	lcd_goto_xy(0,1);
	for(i=0;i<5;i++)
	{
		// get characters[0] to characters[7]
		//print_character('0'+values[i] * 10 /(max+1)); // for deugging, shows percentage instead of graph
		print_character(characters[values[i]*8/(max+1)]);
	}
}

void check_for_qtr_shorts()
{
	// Test for shorts
	unsigned int sensor_pins[] = {IO_C0,IO_C1,IO_C2,IO_C3,IO_C4};
	unsigned char shorts = 1;
	clear();
	print("qtr");
	lcd_goto_xy(0,1);
	print("shorts");
	while (shorts)
	{
		unsigned char i;
		shorts = 0;
		for(i=0;i<5;i++)
		{
			unsigned char j;
			for (j=0;j<5;j++)
			{
				set_digital_input(sensor_pins[j],PULL_UP_ENABLED);
			}
			set_digital_output(sensor_pins[i],LOW);
			delay_ms(2);
			for (j=0;j<5;j++)
			{
				if (i == j && is_digital_input_high(sensor_pins[j]))
					shorts = 1;
				if (i != j && !is_digital_input_high(sensor_pins[j]))
					shorts = 1;
			}
		}
	}
}

void auto_test_sensor_range()
{
	clear();
	print("AutoTest");
	lcd_goto_xy(0,1);
	print("B");
	while(!button_is_pressed(BUTTON_B));
	while(button_is_pressed(ALL_BUTTONS));

	delay_ms(300);
	clear();
	// Auto calibrate with IR on
	time_reset();
  set_motors(20, -20);
  while(get_ms() < 650)
  calibrate_line_sensors(IR_EMITTERS_ON);
  set_motors(-20, 20);
  while(get_ms() < 1950)
  calibrate_line_sensors(IR_EMITTERS_ON);
  set_motors(20, -20);
  while(get_ms() < 2600)
  calibrate_line_sensors(IR_EMITTERS_ON);
  set_motors(0, 0);

	unsigned int * maximum_calibration_values = get_line_sensors_calibrated_maximum_on();
	unsigned int * minimum_calibration_values = get_line_sensors_calibrated_minimum_on();
	unsigned char k;
	unsigned char readings_ok = 1;
  unsigned char ok_array[5] = {'G','G','G','G','G'};
	for (k=0;k<5;k++)
	{
    if ((maximum_calibration_values[k] - minimum_calibration_values[k]) > 1000)
    {
      if ((minimum_calibration_values[k] * 2) < maximum_calibration_values[k])
      {
        if (minimum_calibration_values[k] < 1600)
        {
           continue;
        }           
        else
        {
          ok_array[k] = 'H';
        }          
      }          
      else
      {
        ok_array[k] = 'M';
      }        
    }        
    else
    {
      ok_array[k] = 'R';
    }      
    readings_ok = 0;
	}
	if (!readings_ok)
	{
		clear();
		print("QTR FAIL");
		lcd_goto_xy(0,1);
		for (k=0;k<5;k++)
		{
      print_character(ok_array[k]);
		}
		print(" C ");
		while(!button_is_pressed(BUTTON_C));
		while(button_is_pressed(ALL_BUTTONS));
	}
}

void sensor_graph()
{
	unsigned int values[5];
	clear();
	while(!button_is_pressed(BUTTON_B))
	{
		read_line_sensors(values,IR_EMITTERS_ON);
		display_values(values,TEST_LINE_SENSOR_TIMEOUT);

		lcd_goto_xy(6,1);
		print("B");

		delay_ms(50);
	}
	while(button_is_pressed(ALL_BUTTONS));
}

void check_emitter_jumper()
{
	unsigned int values[5];
	clear();
	// off values
	while(!button_is_pressed(BUTTON_C))
	{
		read_line_sensors(values,IR_EMITTERS_OFF);

		lcd_goto_xy(0,0);
		print("IR- ");
		display_values(values,TEST_LINE_SENSOR_TIMEOUT);
		lcd_goto_xy(6,1);
		print("C");
		delay_ms(50);
	}
	while(button_is_pressed(ALL_BUTTONS));
}

void test_qtr()
{
	check_for_qtr_shorts();
	auto_test_sensor_range();
	sensor_graph();
	check_emitter_jumper();
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
