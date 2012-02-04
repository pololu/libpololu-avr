/*
  OrangutanServos.h - Library for generating servo control pulses with digital
	outputs on the Orangutan LV, SV, SVP, X2, Baby Orangutan B, or 3pi robot.  Note
	that only the Orangutan SV and SVP can supply enough current to power servos
	off of their regulated voltage.  All other devices can supply the control
	signals only (you must power the servos from a separate source).  This
	library relies on Timer1, so it will conflict with any other libraries that
	use Timer1 (e.g. OrangutanBuzzer).  You cannot use the OrangutanBuzzer
	library to play music while using the OrangutanServo library to control
	servos.
	
	This library can generate up to 16 servo control pulses.  On the Orangutan
	SVP, eight of these pulses must be via the servo pulse mux output.  The other
	eight are optional servo pulse outputs on digital I/O pins themselves.  On
	all other devices, all sixteen servo outputs are on digital I/O pins.
*/

/*
 * Written by Ben Schmidel, August 11, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
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

#ifndef OrangutanServos_h
#define OrangutanServos_h

#ifdef __cplusplus

#include "../OrangutanDigital/OrangutanDigital.h"	// digital I/O routines

// Structure for storing the port register and approrpiate bitmask for an I/O pin.
// This lets us easily change the output value of the pin represented by the struct.
struct PortStruct
{
	volatile unsigned char* portRegister;
	unsigned char bitmask;
};

class OrangutanServos
{
  private:
  
	// helper function that initializes the specified index of the global servo pin array
	inline static void initPortPin(struct PortStruct *port, unsigned char pin)
	{
		struct IOStruct io;
		OrangutanDigital::getIORegisters(&io, pin);
		OrangutanDigital::setOutputValue(&io, 0);
		OrangutanDigital::setDataDirection(&io, 1);
		port->portRegister = io.portRegister;
		port->bitmask = io.bitmask;
	}


  public:

    // constructor (doesn't do anything)
	OrangutanServos();
	
	// frees allocated memory
	~OrangutanServos();
	
	// initializes the global servo pin array with the specified pins, and configures the
	// timer1 hardware module for generating the appropriate servo pulse signals.
	// The Orangutan SVP version of this function takes an array of mux selection pins; the
	// servo signal is output on pin PD5, which is the output of the mux)
	// and uses only one interrupt (when TCNT1 = TOP (ICR1)) while the Orangutan SV, LV, Baby Orangutan, and 3pi version
	// of this function take an array representing up to 8 pins (the pins on which to output
	// the servo signals) and uses two interrupts (when TCNT1 = TOP (ICR1) and when TCNT1 = OCR1A).
	// since the servo code can control up to 16 servos using timer 1, there is an optional second
	// set of parameters that allows the user to specify up to 8 more servos.  The servoPinsB array
	// represents a set of up to eight digital I/O pins on which the servo signals should be output.
	// If you don't want this second set of eight servos, use a numPinsB value of 0 (and you can pass in NULL for servoPinsB).
	// A nonzero return value indicates that memory for the desired arrays could not be allocated
	static unsigned char start(const unsigned char servoPins[], unsigned char numPins, 
		const unsigned char servoPinsB[], unsigned char numPinsB);
	static inline unsigned char start(const unsigned char *servoPins, unsigned char numPins)
	{
		return start(servoPins, numPins, 0, 0);
	}

	// ***NOTE*** use of init() is discouraged; use start() instead
	inline static unsigned char init(const unsigned char servoPins[], unsigned char numPins) __attribute__ ((deprecated))
	{
		return start(servoPins, numPins);
	}
	inline static unsigned char init(const unsigned char servoPins[], unsigned char numPins, 
		const unsigned char servoPinsB[], unsigned char numPinsB) __attribute__ ((deprecated))
	{
		return start(servoPins, numPins, servoPinsB, numPinsB);
	}

	// get the current width of the pulse (in us) being supplied to the specified servo.
	// This method does not rely on feedback from the servo, so if the servo
	// is being restrained or overly torqued, it might not return the actual
	// position of the servo.  If you have speed limiting enabled, you can
	// use this method to determine when the servo pulse signals have reached
	// their desired target widths.
	static unsigned int getServoPosition(unsigned char servoNum);
	
	// send a position value of 0 to turn off the specified servo.  Otherwise, valid
	// target positions are between 400 and 2450 us.
	static void setServoTarget(unsigned char servoNum, unsigned int pos_us);
	
	// get the target position (pulse width in us) of the specified servo.
	static unsigned int getServoTarget(unsigned char servoNum);
	
	// speed parameter is in units of 100ns (1/10th of a microsecond)
	// the servo position will be incremented or decremented by "speed"
	// every 20 ms.
	static void setServoSpeed(unsigned char servoNum, unsigned int speed);
	
	// get the speed of the specified servo (the amount in tenths of a microsecond
	// that the servo position is incremented or decremented every 20 ms).
	static unsigned int getServoSpeed(unsigned char servoNum);
	
	
	// get the current width of the pulse (in us) being supplied to the specified servo.
	// This method does not rely on feedback from the servo, so if the servo
	// is being restrained or overly torqued, it might not return the actual
	// position of the servo.  If you have speed limiting enabled, you can
	// use this method to determine when the servo pulse signals have reached
	// their desired target widths.
	static unsigned int getServoPositionB(unsigned char servoNum);
	
	// send a position value of 0 to turn off the specified servo.  Otherwise, valid
	// target positions are between 400 and 2450 us.
	static void setServoTargetB(unsigned char servoNum, unsigned int pos_us);
	
	// get the target position (pulse width in us) of the specified servo.
	static unsigned int getServoTargetB(unsigned char servoNum);
	
	// speed parameter is in units of 100ns (1/10th of a microsecond)
	// the servo position will be incremented or decremented by "speed"
	// every 20 ms.
	static void setServoSpeedB(unsigned char servoNum, unsigned int speed);
	
	// get the speed of the specified servo (the amount in tenths of a microsecond
	// that the servo position is incremented or decremented every 20 ms).
	static unsigned int getServoSpeedB(unsigned char servoNum);
	
	// disable timer interrupt and stop generating pulses (leave lines driving low)
	static void stop();
};

extern "C" {
#endif // __cplusplus

// use of servos_init() is discouraged; use servos_start() instead
unsigned char servos_init(const unsigned char servoPins[], unsigned char numPins) __attribute__ ((deprecated));

// use of servos_init_extended() is discouraged; use servos_start_extended() instead
unsigned char servos_init_extended(const unsigned char servoPins[], unsigned char numPins, 
	const unsigned char servoPinsB[], unsigned char numPinsB) __attribute__ ((deprecated));

unsigned char servos_start(const unsigned char servoPins[], unsigned char numPins);

unsigned char servos_start_extended(const unsigned char servoPins[], unsigned char numPins, 
	const unsigned char servoPinsB[], unsigned char numPinsB);

unsigned int get_servo_position(unsigned char servoNum);

void set_servo_target(unsigned char servoNum, unsigned int pos_us);

unsigned int get_servo_target(unsigned char servoNum);

void set_servo_speed(unsigned char servoNum, unsigned int speed);

unsigned int get_servo_speed(unsigned char servoNum);

unsigned int get_servo_positionB(unsigned char servoNum);
static inline unsigned int get_servo_position_b(unsigned char servoNum)
{
	return get_servo_position_b(servoNum);
}

void set_servo_targetB(unsigned char servoNum, unsigned int pos_us);
static inline void set_servo_target_b(unsigned char servoNum, unsigned int pos_us)
{
	set_servo_targetB(servoNum, pos_us);
}

unsigned int get_servo_targetB(unsigned char servoNum);
static inline unsigned int get_servo_tartget_b(unsigned char servoNum)
{
	return get_servo_targetB(servoNum);
}

void set_servo_speedB(unsigned char servoNum, unsigned int speed);
static inline void set_servo_speed_b(unsigned char servoNum, unsigned int speed)
{
	set_servo_speedB(servoNum, speed);
}

unsigned int get_servo_speedB(unsigned char servoNum);
static inline unsigned int get_servo_speed_b(unsigned char servoNum)
{
	return get_servo_speedB(servoNum);
}

void servos_stop(void);

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
