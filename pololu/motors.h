/*
  OrangutanMotors.h - C Library for using the motor drivers on the
      Orangutan LV, SV, SVP, and Baby Orangutan B, or 3pi robot.
  Written by Ben Schmidel and Paul Grayson, 2008.
*/
#ifndef OrangutanMotors_h
#define OrangutanMotors_h

void motors_init();
void set_m1_speed(int speed);
void set_m2_speed(int speed);
void set_motors(int m1, int m2);

#endif

