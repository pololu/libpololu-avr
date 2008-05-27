/*
  OrangutanMotors.cpp - Library for using the motor drivers on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
  Written by Ben Schmidel, May 15, 2008.
  Released into the public domain.
*/

#ifndef F_CPU
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU

#include <avr/interrupt.h>
#include <avr/io.h>
#include "OrangutanMotors.h"

/*  This comment block will efficiently count elapsed milliseconds
volatile unsigned long msCounter;	// returned by OrangutanMotors::millis()
unsigned int us_times_10 = 0;		// in units of 10^-7 s


// this ISR is called every time timer2 overflows
ISR(TIMER2_OVF_vect)
{
	us_times_10 += 1024;
	if (us_times_10 >= 10000)
	{
		msCounter++;
		us_times_10 -= 10000;
	}
}
*/

extern volatile unsigned long timer0_overflow_count;
unsigned char miniCount = 0;

#ifndef LIB_ORANGUTAN
// this ISR is called every time timer2 overflows.
// it replaces the timer0 overflow ISR that drives millis()
// so that millis() and delay() will still work
ISR(TIMER2_OVF_vect)
{
	miniCount++;
	if (miniCount == 8)
	{
		timer0_overflow_count++;
		miniCount = 0;
	}
}
#endif

#ifdef LIB_ORANGUTAN

extern "C" void motors_init()
{
  OrangutanMotors.init();
}

extern "C" void set_m1_speed(int speed)
{
  OrangutanMotors.setM1Speed(speed);
}

extern "C" void set_m2_speed(int speed)
{
  OrangutanMotors.setM2Speed(speed);
}

extern "C" void set_motors(int m1, int m2)
{
  OrangutanMotors.setSpeeds(m1, m2);
}

#endif


// constructor

OrangutanMotors::OrangutanMotors()
{
}


// initialize timers 0 and 2 to generate the proper PWM ouputs
// to the motor drivers
void OrangutanMotors::init2()
{
	TIMSK0 &= ~(1 << TOIE0);	// timer0 overflow interrupt disabled
	TIMSK2 |= 1 << TOIE2;		// timer2 overflow interrupt enabled
	sei();						// global interrupt enable

	// configure for inverted PWM output on motor control pins:   
    //  set OCxx on compare match, clear on timer overflow   
    //  Timer0 and Timer2 count up from 0 to 255   
    TCCR0A = TCCR2A = 0xF3;
  
    // use the system clock/8 (=2.5 MHz) as the timer clock,
	// which will produce a PWM frequency of 10 kHz
    TCCR0B = TCCR2B = 0x02;

	// use the system clock (=20 MHz) as the timer clock,
	// which will produce a PWM frequency of 78 kHz.  The Baby Orangutan B
	// and 3Pi can support PWM frequencies this high.  The
	// Orangutan LV-168 cannot support frequencies above 10 kHz.
    //TCCR0B = TCCR2B = 0x01;

    // initialize all PWMs to 0% duty cycle (braking)   
    OCR0A = OCR0B = OCR2A = OCR2B = 0;   
  
    // set PWM pins as digital outputs (the PWM signals will not   
    // appear on the lines if they are digital inputs)   
    DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);   
    DDRB |= (1 << PB3);
}


// sets the motor speed.  The sign of 'speed' determines the direction
// and the magnitude determines the speed.  limits: -255 <= speed < 255
// |speed| = 255 produces the maximum speed while speed = 0 is full brake.
void OrangutanMotors::setM1Speed(int speed)
{
	init();
	unsigned char reverse = 0;

	if (speed < 0)
	{
		speed = -speed;	// make speed a positive quantity
		reverse = 1;	// preserve the direction
	}
	if (speed > 0xFF)	// 0xFF = 255
		speed = 0xFF;

	if (reverse)
	{
		OCR0B = 0;		// hold one driver input high
		OCR0A = speed;	// pwm the other input
	}
	else	// forward
	{
		OCR0B = speed;	// pwm one driver input
		OCR0A = 0;		// hold the other driver input high
	}
}


void OrangutanMotors::setM2Speed(int speed)
{
	init();
	unsigned char reverse = 0;

	if (speed < 0)
	{
		speed = -speed;	// make speed a positive quantity
		reverse = 1;	// preserve the direction
	}
	if (speed > 0xFF)	// 0xFF = 255
		speed = 0xFF;

	if (reverse)
	{
		OCR2B = 0;		// hold one driver input high
		OCR2A = speed;	// pwm the other input
	}
	else	// forward
	{
		OCR2B = speed;	// pwm one driver input
		OCR2A = 0;		// hold the other driver input high
	}
}


void OrangutanMotors::setSpeeds(int m1Speed, int m2Speed)
{
	setM1Speed(m1Speed);
	setM2Speed(m2Speed);
}
