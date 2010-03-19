#include <pololu/orangutan.h>

/*
 * stepper-motor1: for the Orangutan LV, SV, SVP, X2, Baby-O.
 *
 * This sample program lets you control a stepper motor using the motor driver
 * outputs of any Orangutan robot controller.  The two outputs of motor channel one
 * should connect to one stepper motor coil and the two outputs of motor channel two
 * should connect to the other stepper motor coil.  The one_step() function takes
 * changes the motor driver state to the next step in the specified direction, and
 * it supports full stepping, half stepping, or quarter stepping.  The motor driver
 * outputs are PWMed to keep the total current constant, which keeps the torque
 * approximately constant from step to step.  Half stepping adds steps between each
 * of the full steps, and quarter stepping adds steps between each of the half steps.
 * If your stepper motor has 200 steps per revolution, you have 200 full steps/rev,
 * 400 half steps/rev, and 800 quarter steps/rev. Using quarter stepping gives you
 * more resolution and can produce smoother movement, but it probably won't work well
 * for rapid rotation as the PWM frequency used to achieve the quarter steps is only
 * 10 kHz.  If the step period is the same order of magnitude as the PWM period,
 * this method of quarter stepping shouldn't be used.
 *
 * The multistep() function is a blocking function that repeatedly calls one_step()
 * with loop delays between steps to achieve the desired rotation speed.  You could
 * modify this example to call one_step() at a specified interval from a timer
 * interrupt, which would allow your main program to continue running while the
 * stepper motor is turning.
 *
 * The stepper motor coils must be energized for it to maintain its holding torque.
 * If you want to save power and don't need holding torque to keep the stepper
 * motor in place, you can call the stepper_off() function.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#define QUARTER_STEP	1
#define HALF_STEP		2
#define FULL_STEP		4

unsigned char stepMode = FULL_STEP;


// Advances the stepper motor by one step either clockwise or counterclockwise
// with the direction specified by the argument dir (0 or 1).  The size of the
// step depends on stepMode and can either be a full step, a half step, or a
// quarter step.  Full stepping is produced by repeating a four-state cycle
// in which both coils are always energized to carry the same magnitude of current
// but the direction of the current is sequentially switched.  Running through the
// four-state cycle in the reverse order reverses the direction of rotation.  The
// general equation for coil current should be as follows:
// coil 1 current = I * sin(a)
// coil 2 current = I * cos(a)
// When full stepping, the four states are: 
// forwards: a = 0, 90, 180, 270 degrees
// reverse:  a = 0, 270, 180, 90 degrees
// half stepping comes from: a = 0, 45, 90, 135, 180, 225, 270, 315 degrees
// quarter stepping comes from a = the 16 multiples of 22.5 from 22.5 to 360 deg
void one_step(unsigned char dir)
{
	// this static variable lets us remember what step we're on so we
	// can change to the appropriate next state in the sequence
	static unsigned char step = 0;

	// compute the next step based on the direction argument dir
	// and the step mode.  Full stepping skips half and quarter steps,
	// and half stepping skips quarter steps.  Quarter stepping cycles
	// through all 16 steps.
	if (dir == 1)
		step += stepMode;
	else
		step -= stepMode;

	switch (step & 15)
	{
		case 0:	// full step (both coils energized at 71%)
			set_motors(180, 180);
			break;
		case 1:	// quarter step (coil 1 at 38% and coil 2 at 92%)
			set_motors(98, 236);
			break;
		case 2: // half step (coil 1 at 0% and coil 2 at 100%)
			set_motors(0, 255);
			break;
		case 3: // quarter step
			set_motors(-98, 236);
			break;
		case 4: // full step
			set_motors(-180, 180);
			break;
		case 5: // quarter step
			set_motors(-236, 98);
			break;
		case 6: // half step
			set_motors(-255, 0);
			break;
		case 7: // quarter step
			set_motors(-236, -98);
			break;
		case 8: // full step
			set_motors(-180, -180);
			break;
		case 9: // quarter step
			set_motors(-98, -236);
			break;
		case 10: // half step
			set_motors(0, -255);
			break;
		case 11: // quarter step
			set_motors(98, -236);
			break;
		case 12: // full step
			set_motors(180, -180);
			break;
		case 13: // quarter step
			set_motors(236, -98);
			break;
		case 14: // half step
			set_motors(255, 0);
			break;
		case 15: // quarter step
			set_motors(236, 98);
			break;
	}
}


// This is a blocking function that repeatedly takes a single step and then
// delays for step_delay_us microseconds.  When it finishes, the stepper motor
// coils will continued to be energized according to the final step so that
// the stepper motor maintains its position and holding torque.  The maximum
// time per step possible with this function is 65.535 ms.
void multistep(int steps, unsigned int step_delay_us)
{
	unsigned char dir = 1;
	if (steps < 0)
	{
		dir = 0;
		steps = -steps;
	}

	while (steps--)
	{
		one_step(dir);
		delay_us(step_delay_us);
	}
}

// Stop delivering current to the stepper motor coils.  This will conserve
// power, but the holding torque will be zero after this function is called.
void stepper_off()
{
	set_motors(0, 0);
}


int main()
{
	while (1)
	{
		if (stepMode == FULL_STEP)
			stepMode = HALF_STEP;
		else
			stepMode = FULL_STEP;
		multistep(400, 5000);
		delay_ms(500);
		unsigned char i;
		for (i = 0; i < 4; i++)
		{
			multistep(-100, 2000);
			delay_ms(100);
		}
	}
}
