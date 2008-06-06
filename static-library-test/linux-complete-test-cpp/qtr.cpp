#include <pololu/orangutan>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "assert.h"

const char bars[] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

const char left_bar[] PROGMEM = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000
};

const char right_bar[] PROGMEM = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00000
};

void display_values(unsigned int *values, unsigned int max)
{
  unsigned char i;

  unsigned char characters[] = {' ',0,1,2,3,4,5,255};

  OrangutanLCD::gotoXY(0,1);
  for(i=0;i<5;i++)
  {
    // get characters[0] to characters[7]
    OrangutanLCD::print(characters[values[i]*8/(max+1)]);
  }
}

unsigned char pins[] = {14,15,16,17,18};
PololuQTRSensorsRC qtr(pins,5,2000,19);

void test_qtr()
{
  OrangutanLCD::clear();

  // Load bar graph characters.
  // Together with space and the solid block at 255, this makes almost
  // all possible bar heights, with two to spare.
  unsigned char i;
  for(i=0;i<6;i++)
  {
    OrangutanLCD::loadCustomCharacter(bars+i,i);
  }
  OrangutanLCD::loadCustomCharacter(left_bar,6);
  OrangutanLCD::loadCustomCharacter(right_bar,7);

  // test RC
  unsigned int values[5];

  // raw values
  while(!OrangutanPushbuttons::isPressed(ALL_BUTTONS))
  {
    OrangutanLCD::clear();

    qtr.read(values);

    printf("Raw %4ud",values[0]);

    display_values(values,2000);
    //    qtr.calibrate();
    OrangutanDelay::ms(50);
  }
 
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  // off values
  while(!OrangutanPushbuttons::isPressed(ALL_BUTTONS))
  {
    OrangutanLCD::clear();

    qtr.read(values,QTR_EMITTERS_OFF);

    printf("Off %4ud",values[0]);

    display_values(values,2000);
    //    qtr.calibrate();
    OrangutanDelay::ms(50);
  }

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  // raw values - off values
  while(!OrangutanPushbuttons::isPressed(ALL_BUTTONS))
  {
    OrangutanLCD::clear();

    qtr.read(values,QTR_EMITTERS_ON_AND_OFF);

    printf("Sub %4ud",values[0]);

    display_values(values,2000);
    //    qtr.calibrate();
    OrangutanDelay::ms(50);
  }

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  // calibrated values
  while(!OrangutanPushbuttons::isPressed(ALL_BUTTONS))
  {
    qtr.readCalibrated(values);

    OrangutanLCD::clear();
    printf("Cal %4ud",values[0]);

    display_values(values,1000);
    OrangutanDelay::ms(50);
  }

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

  // line detection
  while(!OrangutanPushbuttons::isPressed(ALL_BUTTONS))
  {
    unsigned int pos = qtr.readLine(values);

    OrangutanLCD::clear();

    // we want pos = 0-499 to print zero spaces, etc
    unsigned char spaces = (pos+500)/1000;
    for(i=0;i<spaces;i++)
      OrangutanLCD::print(' ');

    // print the bar to the left, middle, or right, as appropriate
    if(pos == 0)
      OrangutanLCD::print((char)6); // all the way on the left
    else if(pos == 4000)
      OrangutanLCD::print((char)7); // all the way on the right
    else if((pos+500)%1000 <= 333)
      OrangutanLCD::print((char)6);
    else if((pos+500)%1000 <= 666)
      OrangutanLCD::print('|');
    else OrangutanLCD::print((char)7);

    display_values(values,1000);
    OrangutanDelay::ms(50);
  }

  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);

}
