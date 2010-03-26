#include <pololu/orangutan.h>

/*
 * svp-sixteen-servo: for the Orangutan SVP.
 *
 * This example uses the OrangutanServos functions to control sixteen servos.
 * To use this example, you must connect the correct AVR I/O pins to their
 * corresponding servo demultiplexer output-selection pins.
 *   - Connect PB3 to SA.
 *   - Connect PB4 to SB.
 *   - Connect PC0 to SC.
 * Servos a0-a7 will be on the servo demux outputs.
 * Servos b0-b7 will be on pins PA0-PA7.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
	// This array specifies the correspondence between I/O pins and DEMUX
	// output-selection pins.  This demo uses three pins, which allows you
	// to control up to 8 servos from the demux.
	const unsigned char demuxPins[] = {IO_B3, IO_B4, IO_C0}; // B3=SA, B4=SB, C0=B5.

	// This array specifies the correspondence between I/O pins and
	// software-PWMed servos.
	const unsigned char servoPinsB[] = {IO_A0, IO_A1, IO_A2, IO_A3, IO_A4, IO_A5, IO_A6, IO_A7};

	servos_start_extended(demuxPins, sizeof(demuxPins), servoPinsB, sizeof(servoPinsB));

	// Set the servo speed to 150.  This means that the pulse width
	// will change by at most 15 microseconds every 20 ms.  So it will
	// take 1.33 seconds to go from a pulse width of 1000 us to 2000 us.
	set_servo_speed(0, 150);   // Servo a0 = Demux output 0
	set_servo_speed(1, 150);   // Servo a1 = Demux output 1
	set_servo_speed(2, 150);   // Servo a2 = Demux output 2
	set_servo_speed(3, 150);   // Servo a3 = Demux output 3
	set_servo_speed(4, 150);   // Servo a4 = Demux output 4
	set_servo_speed(5, 150);   // Servo a5 = Demux output 5
	set_servo_speed(6, 150);   // Servo a6 = Demux output 6
	set_servo_speed(7, 150);   // Servo a7 = Demux output 7
	set_servo_speed_b(0, 150);  // Servo b0 = pin A0
	set_servo_speed_b(1, 150);  // Servo b1 = pin A1
	set_servo_speed_b(2, 150);  // Servo b2 = pin A2
	set_servo_speed_b(3, 150);  // Servo b3 = pin A3
	set_servo_speed_b(4, 150);  // Servo b4 = pin A4
	set_servo_speed_b(5, 150);  // Servo b5 = pin A5
	set_servo_speed_b(6, 150);  // Servo b6 = pin A6
	set_servo_speed_b(7, 150);  // Servo b7 = pin A7

	// Make all the servos go to a neutral position.
	set_servo_target(0, 1300);
	set_servo_target(1, 1300);
	set_servo_target(2, 1300);
	set_servo_target(3, 1300);
	set_servo_target(4, 1300);
	set_servo_target(5, 1300);
	set_servo_target(6, 1300);
	set_servo_target(7, 1300);
	set_servo_target_b(0, 1300);
	set_servo_target_b(1, 1300);
	set_servo_target_b(2, 1300);
	set_servo_target_b(3, 1300);
	set_servo_target_b(4, 1300);
	set_servo_target_b(5, 1300);
	set_servo_target_b(6, 1300);
	set_servo_target_b(7, 1300);

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
			set_servo_target(7, 1800); delay_ms(350);
			set_servo_target_b(0, 1800); delay_ms(350);
			set_servo_target_b(1, 1800); delay_ms(350);
			set_servo_target_b(2, 1800); delay_ms(350);
			set_servo_target_b(3, 1800); delay_ms(350);
			set_servo_target_b(4, 1800); delay_ms(350);
			set_servo_target_b(5, 1800); delay_ms(350);
			set_servo_target_b(6, 1800); delay_ms(350);
			set_servo_target_b(7, 1800); delay_ms(1000);
			
			set_servo_target(0, 1300); delay_ms(350);
			set_servo_target(1, 1300); delay_ms(350);
			set_servo_target(2, 1300); delay_ms(350);
			set_servo_target(3, 1300); delay_ms(350);
			set_servo_target(4, 1300); delay_ms(350);
			set_servo_target(5, 1300); delay_ms(350);
			set_servo_target(6, 1300); delay_ms(350);
			set_servo_target(7, 1300); delay_ms(350);
			set_servo_target_b(0, 1300); delay_ms(350);
			set_servo_target_b(1, 1300); delay_ms(350);
			set_servo_target_b(2, 1300); delay_ms(350);
			set_servo_target_b(3, 1300); delay_ms(350);
			set_servo_target_b(4, 1300); delay_ms(350);
			set_servo_target_b(5, 1300); delay_ms(350);
			set_servo_target_b(6, 1300); delay_ms(350);
			set_servo_target_b(7, 1300); delay_ms(350);
		}
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
