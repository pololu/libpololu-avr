#include <OrangutanPushbuttons.h>
#include <OrangutanLCD.h>

/*
 * OrangutanLCDExample2 for the Orangutan LV-168
 *
 * This example program is indended for use on the Orangutan LV-168.
 * It uses the OrangutanLCD library to display custom characters on 
 * the LCD.  Simply push a any user pushbutton to display a new,
 * randomly chosen, custom mood characer.
 *
 * http://www.pololu.com/docs/0J17/5.c
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLCD lcd;

// define some custom "mood" characters
#include <avr/pgmspace.h>  // this lets us refer to data in program space (i.e. flash)
const char happy[] PROGMEM = {
  0b00000,                 // the five bits that make up the top row of the 5x8 character
  0b01010,
  0b01010,
  0b01010,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

const char sad[] PROGMEM = {
  0b00000, 
  0b01010,
  0b01010,
  0b01010,
  0b00000,
  0b01110,
  0b10001,
  0b00000
};

const char indifferent[] PROGMEM = {
  0b00000, 
  0b01010,
  0b01010,
  0b01010,
  0b00000,
  0b00000,
  0b01110,
  0b00000
};

const char surprised[] PROGMEM = {
  0b00000, 
  0b01010,
  0b01010,
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b01110
};

const char mocking[] PROGMEM = {
  0b00000, 
  0b01010,
  0b01010,
  0b01010,
  0b00000,
  0b11111,
  0b00101,
  0b00010
};

char prevMood = 5;


void setup()                    // run once, when the sketch starts
{
  lcd.loadCustomCharacter(happy, 0);
  lcd.loadCustomCharacter(sad, 1);
  lcd.loadCustomCharacter(indifferent, 2);
  lcd.loadCustomCharacter(surprised, 3);
  lcd.loadCustomCharacter(mocking, 4);
  lcd.clear();                  // this must be called before we can use the custom characters
  lcd.print("mood: ?");
}

void loop()                     // run over and over again
{
  OrangutanPushbuttons::waitForButton(ALL_BUTTONS);   // wait for any button to be pressed
  
  lcd.gotoXY(6, 0);             // move cursor to the correct position
  
  char mood;
  do
  {
    mood = (char)random(5);
  } while (mood == prevMood);   // ensure we get a new mood that differs from the previous
  prevMood = mood;
  
  lcd.print(mood);   // print a random mood character
}
