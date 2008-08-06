/*
  OrangutanPushbuttons.h - Library for using the thee user pushbuttons on the
      Orangutan LV-168
  Written by Ben Schmidel, May 23, 2008.
*/
#ifndef OrangutanPushbuttons_h
#define OrangutanPushbuttons_h

void buttons_init();
unsigned char wait_for_button_press(unsigned char buttons);
unsigned char wait_for_button_release(unsigned char buttons);
unsigned char wait_for_button(unsigned char buttons);
unsigned char button_is_pressed(unsigned char buttons);

// LV-168 definitions
#define TOP_BUTTON	(1 << 5)	// PB1; Arduino pin 13
#define MIDDLE_BUTTON	(1 << 4)	// PB4; Arduino pin 12
#define BOTTOM_BUTTON	(1 << 1)	// PB5; Arduino pin 9

// 3pi definitions
#define BUTTON_C	(1 << 5)	// PB1; Arduino pin 13
#define BUTTON_B	(1 << 4)	// PB4; Arduino pin 12
#define BUTTON_A	(1 << 1)	// PB5; Arduino pin 9

#define ALL_BUTTONS	(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON)

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
