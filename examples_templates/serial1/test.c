#include <pololu/orangutan.h>  
  
/*  
 * serial1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example listens for bytes on PD0/RXD.  Whenever it receives a byte, it
 * performs a custom action.  Whenever the user presses the middle button, it
 * transmits a greeting on PD1/TXD.
 *
 * The Baby Orangutan does not have a green LED, LCD, or pushbuttons so
 * that part of the code will not work.
 *
 * To make this example compile for the Orangutan SVP or X2, you
 * must add a first argument of UART0 to all the serial_* function calls
 * to receive on PD0/RXD0 and transmit on PD1/TXD0.
 * 
 * http://www.pololu.com/docs/0J20 
 * http://www.pololu.com  
 * http://forum.pololu.com  
 */   

// receive_buffer: A ring buffer that we will use to receive bytes on PD0/RXD.
// The OrangutanSerial library will put received bytes in to
// the buffer starting at the beginning (receiveBuffer[0]).
// After the buffer has been filled, the library will automatically
// start over at the beginning.
char receive_buffer[32];

// receive_buffer_position: This variable will keep track of which bytes in the receive buffer
// we have already processed.  It is the offset (0-31) of the next byte
// in the buffer to process.
unsigned char receive_buffer_position = 0;

// send_buffer: A buffer for sending bytes on PD1/TXD.
char send_buffer[32];

// wait_for_sending_to_finish:  Waits for the bytes in the send buffer to
// finish transmitting on PD1/TXD.  We must call this before modifying
// send_buffer or trying to send more bytes, because otherwise we could
// corrupt an existing transmission.
void wait_for_sending_to_finish()
{
	while(!serial_send_buffer_empty());
}

// process_received_byte: Responds to a byte that has been received on
// PD0/RXD.  If you are writing your own serial program, you can
// replace all the code in this function with your own custom behaviors.
void process_received_byte(char byte)
{
	switch(byte)
	{
		// If the character 'G' is received, turn on the green LED.
		case 'G':
			green_led(1);
			break;

		// If the character 'g' is received, turn off the green LED.
		case 'g':
			green_led(0);
			break;

		// If the character 'c' is received, play the note c.
		case 'c':
			play_from_program_space(PSTR("c16"));
			break;

		// If the character 'd' is received, play the note d.
		case 'd':
			play_from_program_space(PSTR("d16"));
			break;

		// If any other character is received, change its capitalization and
		// send it back.
		default:
			wait_for_sending_to_finish();
			send_buffer[0] = byte ^ 0x20;
			serial_send(send_buffer, 1);
			break;
	}
}

void check_for_new_bytes_received()
{
	while(serial_get_received_bytes() != receive_buffer_position)
	{
		// Process the new byte that has just been received.
		process_received_byte(receive_buffer[receive_buffer_position]);

		// Increment receive_buffer_position, but wrap around when it gets to
		// the end of the buffer. 
		if (receive_buffer_position == sizeof(receive_buffer)-1)
		{
			receive_buffer_position = 0;
		}
		else
		{
			receive_buffer_position++;
		}
	}
}

int main()
{
	// Set the baud rate to 9600 bits per second.  Each byte takes ten bit
	// times, so you can get at most 960 bytes per second at this speed.
	serial_set_baud_rate(9600);

	// Start receiving bytes in the ring buffer.
	serial_receive_ring(receive_buffer, sizeof(receive_buffer));

    while(1)
    {
		// Deal with any new bytes received.
		check_for_new_bytes_received();

		// If the user presses the middle button, send "Hi there!"
		// and wait until the user releases the button.
		if (button_is_pressed(MIDDLE_BUTTON))
		{
			wait_for_sending_to_finish();
			memcpy_P(send_buffer, PSTR("Hi there!\r\n"), 11);
			serial_send(send_buffer, 11);

			// Wait for the user to release the button.  While the processor is
			// waiting, the OrangutanSerial library will take care of receiving
			// bytes using the serial reception interrupt.  But if enough bytes
			// arrive during this period to fill up the receive_buffer, then the
			// older bytes will be lost and we won't know exactly how many bytes
			// have been received.
			wait_for_button_release(MIDDLE_BUTTON);
		}
    }
}
