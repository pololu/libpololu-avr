#include <pololu/orangutan>
#include <stdio.h>
#include "assert.h"

extern OrangutanMotors motors;

void test_motors()
{
  OrangutanLCD::clear();
  printf("Motors\nready?");
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("M1\nslow");
  OrangutanMotors::setSpeeds(60,0);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("M1\nfast");
  motors.setM1Speed(255);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("M1\nbk slow");
  OrangutanMotors::setM1Speed(-60);
  OrangutanMotors::setM2Speed(0);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("M1\nbk fast");
  motors.setSpeeds(-255,0);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  /// right motor
  OrangutanLCD::clear();
  printf("M2\nslow");
  OrangutanMotors::setSpeeds(0,60);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("M2\nfast");
  motors.setM2Speed(255);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("Right\nbk slow");
  OrangutanMotors::setSpeeds(0,-60);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanLCD::clear();
  printf("Right\nbk fast");
  motors.setSpeeds(0,-255);
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  OrangutanMotors::setM1Speed(0);
  motors.setM2Speed(0);
}
