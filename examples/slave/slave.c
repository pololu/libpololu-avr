#include <pololu/3pi.h>
#include <avr/io.h>

/*
 */

void send_sensor_values()
{
	char message[10];
	read_line_sensors((unsigned int *)message, IR_EMITTERS_ON);
	serial_send_blocking(message, 10);
}

// sends the batter voltage in millivolts
void send_battery_millivolts()
{
	int message[1];
	message[0] = read_battery_millivolts();
	serial_send_blocking((char *)message, 2);
}

// a ring buffer for data coming in
char buffer[100];

// a pointer to where we are reading from
unsigned char read_index = 0;

// Waits for the next byte and returns it
char read_next_byte()
{
	while(serial_get_received_bytes() == read_index);
	char ret = buffer[read_index];
	read_index ++;
	if(read_index >= 100)
		read_index = 0;
	return ret;
}

// Backs up by one byte in the ring buffer
void previous_byte()
{
	read_index --;
	if(read_index == 255)
		read_index = 99;
}

char is_command(char byte)
{
	if (byte < 0)
		return 1;
	return 0;
}

char is_data(char byte)
{
	if (byte < 0)
		return 0;
	return 1;
}

// If it's not a data byte, beeps, backs up one, and returns true.
char check_data_byte(char byte)
{
	if(is_data(byte))
		return 0;
	
	play("o3c");

	clear();
	print("Bad data");
	lcd_goto_xy(0,1);
	print_hex_byte(byte);

	previous_byte();
	return 1;
}

void m1_forward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m1_speed(byte*2);
}

void m2_forward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m2_speed(byte*2);
}

void m1_backward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m1_speed(-byte*2);
}

void m2_backward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m2_speed(-byte*2);
}

char music_buffer[100];

void do_play()
{
	unsigned char tune_length = read_next_byte();

	if(check_data_byte(tune_length))
		return;

	unsigned char i;
	for(i=0;i<tune_length;i++)
	{
		if(i > sizeof(music_buffer)) // avoid overflow
			return;

		music_buffer[i] = read_next_byte();

		if(check_data_byte(music_buffer[i]))
			return;
	}

	green_led(1);

	// add the end of string character 0
	music_buffer[i] = 0;
	
	play(music_buffer);
}

int main()
{
	pololu_3pi_init(2000);  

	clear();
	print("Slave");

	// configure serial clock for 115.2 kbaud
	serial_set_baud_rate(115200);

	// start receiving into the ring buffer
	serial_receive_ring(buffer, 100);

	while(1)
	{
		// wait for a command
		char command = read_next_byte();
		switch(command)
		{
		case (char)0x00:
			// slient error - probable master resetting
			break;

		case (char)0x81:
			serial_send_blocking("3pi0.9", 6);
			break;
		case (char)0x86:
			send_sensor_values();
			break;

		case (char)0xB0:
			//send_trimpot();
			break;
		case (char)0xB1:
			send_battery_millivolts();
			break;
		case (char)0xB3:
			do_play();
			break;

		case (char)0xC1:
			m1_forward();
			break;
		case (char)0xC2:
			m1_backward();
			break;
		case (char)0xC5:
			m2_forward();
			break;
		case (char)0xC6:
			m1_backward();
			break;

		default:
			clear();
			print("Bad cmd");
			lcd_goto_xy(0,1);
			print_hex_byte(command);

			play("o7l16crcrc");
			continue; // bad command
		}
	}
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
