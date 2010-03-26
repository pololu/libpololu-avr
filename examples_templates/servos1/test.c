#include <pololu/orangutan.h>

/*
 * servos1: for all Orangutans (LV, SV, SVP, X2, Baby) and the 3pi robot.
 *
 * This example uses the OrangutanServos functions to control two servos
 * connected to pins PD0 and PD1.  The servo pulses are generated in the
 * background using interrupts, so program execution is not blocked by
 * the pulse-generation code.  If you are using the Orangutan SV or SVP,
 * you can power the servos directly from the board's 5V bus.  For all
 * other Orangutans (LV, X2, Baby) and the 3pi robot, you need to power
 * the servos from a separate source since the on-board voltage regulators
 * do not have enough power to drive typical servos.  This example uses
 * pushbutton inputs to trigger events, so you will need connect an external
 * pushbutton (or wire) to the correct pin if if you are using a Baby
 * Orangutan.
 * 
 * The program will hold the two servos connected to pins PD0 and PD1 at
 * default positions unless a user pushbutton is pressed.  While the button
 * is held down, the servos will slowly move to new positions.  When the
 * button is released, the servos will quickly go back to their default
 * positions.
 *
 * Note: the "B" version of the servo functions are used in this example
 * so that it behaves the same for the Orangutan SVP as for all other
 * Orangutans.  If you are not using the Orangutan SVP, you can do not
 * need to use the B versions of the functions (e.g. you can use
 * set_servo_target(0, 1300) instead of set_servo_target_b(0, 1300)).
 * If you do this, you should replace the call to servos_start_extended()
 * with servos_starts().
 *
 * The OrangutanServo library uses timer 1, so it conflicts with the
 * OrangutanBuzzer library, which also uses timer 1.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
	// This array specifies the correspondence between I/O pins and
	// software-PWMed servos.  Servo 0 is on pin D0 and servo 1 is on pin D1.
	const unsigned char servoPinsB[] = {IO_D0, IO_D1};

	servos_start_extended((unsigned char[]){}, 0, servoPinsB, sizeof(servoPinsB));

	while (1)
	{
		set_servo_speed_b(0, 0);	// no speed limit (move as fast as possible)
		set_servo_target_b(0, 1200);
		set_servo_speed_b(1, 0);
		set_servo_target_b(1, 1700);

		// wait for any button to be pressed
		unsigned char button = wait_for_button_press(ANY_BUTTON);

		// set speed of servo 0 to 150, which means that the pulse width
		// will change by at most 15 microseconds every 20 ms, so it will
		// take 800 ms (0.8 s) to go from a position of 1200 to 1800.
		set_servo_speed_b(0, 150);
		set_servo_target_b(0, 1800);	// start servo 0 moving

		// set speed of servo 0 to 60, which means that the pulse width
		// will change by at most 6 microseconds every 20 ms, so it will
		// take 1333 ms (1.3 s) to go from a position of 1300 to 1700.
		set_servo_speed_b(1, 60);
		set_servo_target_b(1, 1300);	// start servo 1 moving

		// wait for pressed button to be released (servos will keep moving)
		wait_for_button_release(button);

		// when button is released, we jump to the top of the loop and the
		// servos positions are reset
	}
}
