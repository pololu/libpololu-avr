#include "../OrangutanMotors/orangutan_motors.h"

int main()
{
  motors_init();
  set_m1_speed(100);
  set_m2_speed(100);
  set_motors(100,-100);
  return 0;
}
