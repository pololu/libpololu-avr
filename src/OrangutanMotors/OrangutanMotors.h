/*
  OrangutanMotors.h - Library for using the motor drivers on the
      Orangutan LV, SV, SVP, Baby Orangutan B, or 3pi robot
*/

/*
 * Written by Ben Schmidel, May 15, 2008.
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
 
#ifndef OrangutanMotors_h
#define OrangutanMotors_h

#ifdef __cplusplus

class OrangutanMotors
{
  public:

    // constructor (doesn't do anything)
	OrangutanMotors();

	// sets the motor speed.  The sign of 'speed' determines the direction
	// and the magnitude determines the speed.  limits: -255 <= speed < 255
	// |speed| = 255 produces the maximum speed while speed = 0 is full brake.
	static void setM1Speed(int speed);
	static void setM2Speed(int speed);
	static void setSpeeds(int m1Speed, int m2Speed);


  private:

	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
  	
	// initializes timers 0 and 2 for proper PWM generation
	static void init2();
};

extern "C" {
#endif // __cplusplus

void motors_init(void);
void set_m1_speed(int speed);
void set_m2_speed(int speed);
void set_motors(int m1, int m2);

#ifdef __cplusplus
}
#endif

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
