#include <pololu/3pi.h>

/*
 */

void send_sensor_values()
{
  char message[10];

  // read 10 bytes directly into the message
  read_line_sensors((unsigned int *)message, IR_EMITTERS_ON);

  // send the message
  serial_send(message, 10);

  // wait for sending before returning
  while(!serial_send_buffer_empty());
}

int main()
{
  char buffer[20];
  pololu_3pi_init(2000);  

  clear();
  print("Slave");

  // configure serial clock for 115.2 kbaud
  serial_set_baud_rate(115200);

  while(1)
  {
    // read up to 20 characters
    serial_receive(buffer, 20);

    // wait for a command
    while(serial_get_received_bytes() == 0);

    if(buffer[0] == (char)0x81)
    {
      green_led(1);
      delay_ms(500);
      green_led(0);
      serial_send("3pi0.9", 6);
    }
    else if(buffer[0] == (char)0x86)
      send_sensor_values();
    else if(buffer[0] == (char)0xC1)
    {
      delay_ms(1);
      // motor 1 forward - wait for a data byte
      while(serial_get_received_bytes() < 2)
      {
	if((unsigned char)buffer[1] >= 0x80)
	  continue; // bad data byte
	set_m1_speed(buffer[1]*2);
	lcd_goto_xy(0,0);
	print_long(buffer[1]*2);
	print("  ");
      }
    }
    else if(buffer[0] == (char)0xC5)
    {
      delay_ms(1);
      // motor 1 forward - wait for a data byte
      while(serial_get_received_bytes() < 2)
      {
	if((unsigned char)buffer[1] >= 0x80)
	  continue; // bad data byte
	set_m2_speed(buffer[1]*2);
	lcd_goto_xy(0,1);
	print_long(buffer[1]*2);
	print("  ");
      }
    }
  }
}
