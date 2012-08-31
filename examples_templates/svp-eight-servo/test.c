#include <pololu/orangutan.h>

/*
 * svp-eight-servo: for the Orangutan SVP.
 *
 * This example uses the OrangutanServos functions to control eight servos.
 * To use this example, you must connect the correct AVR I/O pins to their
 * corresponding servo demultiplexer output-selection pins.
 *   - Connect PB3 to SA.
 *   - Connect PB4 to SB.
 *   - Connect PC0 to SC.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
	// This array specifies the correspondence between I/O pins and DEMUX
	// output-selection pins.  This demo uses three pins, which allows you
	// to control up to 8 servos.  You can also use two, one, or zero pins
	// to control fewer servos.
	const unsigned char demuxPins[] = {IO_B3, IO_B4, IO_C0}; // eight servos
	//const unsigned char demuxPins[] = {IO_B3, IO_B4};      // four servos
	//const unsigned char demuxPins[] = {IO_B3};             // two servos
	//const unsigned char demuxPins[] = {};                  // one servo

	servos_start(demuxPins, sizeof(demuxPins));

	// Set the servo speed to 150.  This means that the pulse width
	// will change by at most 15 microseconds every 20 ms.  So it will
	// take 1.33 seconds to go from a pulse width of 1000 us to 2000 us.
	set_servo_speed(0, 150);
	set_servo_speed(1, 150);
	set_servo_speed(2, 150);
	set_servo_speed(3, 150);
	set_servo_speed(4, 150);
	set_servo_speed(5, 150);
	set_servo_speed(6, 150);
	set_servo_speed(7, 150);

	// Make all the servos go to a neutral position.
	set_servo_target(0, 1300);
	set_servo_target(1, 1300);
	set_servo_target(2, 1300);
	set_servo_target(3, 1300);
	set_servo_target(4, 1300);
	set_servo_target(5, 1300);
	set_servo_target(6, 1300);
	set_servo_target(7, 1300);

	while(1)  // Loop forever.
	{
		// When the user presses the top button, execute a pre-programmed
		// sequence of servo movements.
		if (button_is_pressed(TOP_BUTTON))
		{
			set_servo_target(0, 1800); delay_ms(350);
			set_servo_target(1, 1800); delay_ms(350);
			set_servo_target(2, 1800); delay_ms(350);
			set_servo_target(3, 1800); delay_ms(350);
			set_servo_target(4, 1800); delay_ms(350);
			set_servo_target(5, 1800); delay_ms(350);
			set_servo_target(6, 1800); delay_ms(350);
			set_servo_target(7, 1800); delay_ms(1000);
			
			set_servo_target(0, 1300); delay_ms(350);
			set_servo_target(1, 1300); delay_ms(350);
			set_servo_target(2, 1300); delay_ms(350);
			set_servo_target(3, 1300); delay_ms(350);
			set_servo_target(4, 1300); delay_ms(350);
			set_servo_target(5, 1300); delay_ms(350);
			set_servo_target(6, 1300); delay_ms(350);
			set_servo_target(7, 1300); delay_ms(350);
		}
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
