#include <pololu/3pi.h>
#include <avr/io.h>

/*
 * 3pi-serial-slave - An example serial slave program for the Pololu
 * 3pi Robot.  See README.txt for a complete description.
 *
 * http://www.pololu.com/docs/0J21
 * http://www.pololu.com/docs/0J20
 * http://www.poolu.com/
 * 
 */

// Sends the version of the slave code that is running.
void send_signature()
{
	serial_send_blocking("3pi0.9", 6);
}

// Reads the line sensors and sends their values.  This function can
// do either calibrated or uncalibrated readings.
void send_sensor_values(char calibrated)
{
	char message[10];
	if(calibrated)
		read_line_sensors_calibrated((unsigned int *)message, IR_EMITTERS_ON);
	else
		read_line_sensors((unsigned int *)message, IR_EMITTERS_ON);
	serial_send_blocking(message, 10);
}

// Sends the raw (uncalibrated) sensor values.
void send_raw_sensor_values()
{
	send_sensor_values(0);
}

// Sends the calibated sensor values.
void send_calibrated_sensor_values()
{
	send_sensor_values(1);
}

// Computes the position of a black line using the read_line()
// function, and sends the value.
void send_line_position()
{
	int message[1];
	unsigned int tmp_sensors[5];
	int line_position = read_line(tmp_sensors, IR_EMITTERS_ON);
	message[0] = line_position;

	serial_send_blocking((char *)message, 2);
}

// Sends the trimpot value, 0-1023.
void send_trimpot()
{
	int message[1];
	message[0] = read_trimpot();
	serial_send_blocking((char *)message, 2);
}

// Sends the batter voltage in millivolts
void send_battery_millivolts()
{
	int message[1];
	message[0] = read_battery_millivolts();
	serial_send_blocking((char *)message, 2);
}

// A global ring buffer for data coming in.  This is used by the
// read_next_byte() and previous_byte() functions, below.
char buffer[100];

// A pointer to where we are reading from.
unsigned char read_index = 0;

// Waits for the next byte and returns it.  Runs play_check to keep
// the music playing.
char read_next_byte()
{
	while(serial_get_received_bytes() == read_index)
		play_check();
	char ret = buffer[read_index];
	read_index ++;
	if(read_index >= 100)
		read_index = 0;
	return ret;
}

// Backs up by one byte in the ring buffer.
void previous_byte()
{
	read_index --;
	if(read_index == 255)
		read_index = 99;
}

// Returns true if and only if the byte is a command byte (>= 0x80).
char is_command(char byte)
{
	if (byte < 0)
		return 1;
	return 0;
}

// Returns true if and only if the byte is a data byte (< 0x80).
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

// Drives m1 forward.
void m1_forward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m1_speed(byte == 127 ? 255 : byte*2);
}

// Drives m2 forward.
void m2_forward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m2_speed(byte == 127 ? 255 : byte*2);
}

// Drives m1 backward.
void m1_backward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m1_speed(byte == 127 ? -255 : -byte*2);
}

// Drives m2 backward.
void m2_backward()
{
	char byte = read_next_byte();
	
	if(check_data_byte(byte))
		return;

	set_m2_speed(byte == 127 ? -255 : -byte*2);
}

// A buffer to store the music that will play in the background.
char music_buffer[100];

// Plays a musical sequence.
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
	play_mode(PLAY_CHECK);

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

		// The list of commands is below: add your own simply by
		// choosing a command byte and introducing another case
		// statement.
		switch(command)
		{
		case (char)0x00:
			// slient error - probable master resetting
			break;

		case (char)0x81:
			send_signature();
			break;
		case (char)0x86:
			send_raw_sensor_values();
			break;
		case (char)0x87:
			send_calibrated_sensor_values(1);
			break;
		case (char)0xB0:
			send_trimpot();
			break;
		case (char)0xB1:
			send_battery_millivolts();
			break;
		case (char)0xB3:
			do_play();
			break;
		case (char)0xB4:
			calibrate_line_sensors(IR_EMITTERS_ON);
			send_calibrated_sensor_values(1);
			break;
		case (char)0xB5:
			line_sensors_reset_calibration();
			break;
		case (char)0xB6:
			send_line_position();
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
			m2_backward();
			break;

		default:
			clear();
			print("Bad cmd");
			lcd_goto_xy(0,1);
			print_hex_byte(command);

			play("o7l16crc");
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
