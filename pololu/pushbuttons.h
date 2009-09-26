/*
  OrangutanPushbuttons.h - Library for using the thee user pushbuttons on the
      Orangutan LV, SV, SVP, and 3pi.
  Written by Ben Schmidel, May 23, 2008.
*/
#ifndef OrangutanPushbuttons_h
#define OrangutanPushbuttons_h

#include "OrangutanResources/include/OrangutanModel.h"

void buttons_init();
unsigned char wait_for_button_press(unsigned char buttons);
unsigned char wait_for_button_release(unsigned char buttons);
unsigned char wait_for_button(unsigned char buttons);
unsigned char button_is_pressed(unsigned char buttons);

#ifdef _ORANGUTAN_SVP

// Orangutan SVP definitions
#define TOP_BUTTON		(1 << PORTC5)
#define MIDDLE_BUTTON	(1 << PORTC3)
#define BOTTOM_BUTTON	(1 << PORTC2)

#define BUTTON_DDR		DDRC
#define BUTTON_PORT		PORTC
#define BUTTON_PIN		PINC


#else


// Orangutan LV, SV, and 3pi definitions
#define TOP_BUTTON		(1 << PORTB5)
#define MIDDLE_BUTTON	(1 << PORTB4)
#define BOTTOM_BUTTON	(1 << PORTB1)

#define BUTTON_PORT		PORTB
#define BUTTON_DDR		DDRB
#define BUTTON_PIN		PINB

#endif


#define BUTTON_C		TOP_BUTTON
#define BUTTON_B		MIDDLE_BUTTON
#define BUTTON_A		BOTTOM_BUTTON

#define ALL_BUTTONS		(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON)

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
