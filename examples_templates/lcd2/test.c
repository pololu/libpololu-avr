#include <pololu/orangutan.h>

// get random functions
#include <stdlib.h>

/*
 * lcd2: for the Orangutan controllers and 3pi robot
 *
 * This example program is intended for use on the Orangutan LV/SV-xx8.
 * It uses the OrangutanLCD functions to display custom characters on 
 * the LCD.  Simply push a any user pushbutton to display a new,
 * randomly chosen, custom mood character.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

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


int main()
{
  lcd_load_custom_character(happy, 0);
  lcd_load_custom_character(sad, 1);
  lcd_load_custom_character(indifferent, 2);
  lcd_load_custom_character(surprised, 3);
  lcd_load_custom_character(mocking, 4);
  clear();                  // this must be called before we can use the custom characters
  print("mood: ?");

  // initialize the random number generator based on how long we hold the button the first time
  wait_for_button_press(ALL_BUTTONS);
  long seed = 0;
  while(button_is_pressed(ALL_BUTTONS))
    seed++;
  srandom(seed);

  while(1)
  {
  
    lcd_goto_xy(6, 0);             // move cursor to the correct position
  
    char mood;
    do
    {
      mood = random()%5;
    } while (mood == prevMood);   // ensure we get a new mood that differs from the previous
    prevMood = mood;
  
    print_character(mood);   // print a random mood character
    wait_for_button(ALL_BUTTONS);   // wait for any button to be pressed
  }
}
