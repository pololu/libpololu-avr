/*
  OrangutanMotors.h - C Library for using the motor drivers on the
      Orangutan LV-168 and Baby Orangutan B-48/B-168
  Written by Ben Schmidel and Paul Grayson, 2008.
*/
#ifndef orangutan_motors_h
#define orangutan_motors_h

void motors_init();
void set_m1_speed(int speed);
void set_m2_speed(int speed);
void set_motors(int m1, int m2);

#endif

