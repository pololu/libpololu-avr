/*
  OrangutanMotors.h - Library for using the motor drivers on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
  Written by Ben Schmidel, May 15, 2008.
  Released into the public domain.
*/
#ifndef OrangutanMotors_h
#define OrangutanMotors_h


#define ENABLE_MILLIS	1
#define DISABLE_MILLIS	0



class OrangutanMotors
{
  public:

    // constructor (the argument if the millis() member function will work)
	OrangutanMotors(uint8_t enable_millis);

	// initializes timers 0 and 2 for proper PWM generation
	void init();

	// sets the motor speed.  The sign of 'speed' determines the direction
	// and the magnitude determines the speed.  limits: -255 <= speed < 255
	// |speed| = 255 produces the maximum speed while speed = 0 is full brake.
	void setM1Speed(int16_t speed);
	void setM2Speed(int16_t speed);


  private:

  	uint8_t _enableMillis;		// 0 if this.millis() is to be disabled
								// disabling millis() decreases load on the CPU
};

#endif

