#include <pololu/orangutan.h>
/*
 * buzzer1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * Uses the OrangutanBuzzer library to play a series of notes on the
 * Orangutan's buzzer.  It also uses the OrangutanLCD library to
 * display the notes its playing, and it uses the OrangutanPushbuttons
 * library to allow the user to stop/reset the melody with the top
 * pushbutton.  This is a very simple example that does not take
 * advantage of OrangutanBuzzer's more advanced play() and
 * play_from_program_space() functionality.  This code will work
 * on the Baby Orangutan if you connect an external buzzer to
 * pin PB2.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#define MELODY_LENGTH 95

// These arrays take up a total of 285 bytes of RAM (see buzzer2
// example to learn how to store songs in program memory)
unsigned char note[MELODY_LENGTH] = 
{
  E(5), SILENT_NOTE, E(5), SILENT_NOTE, E(5), SILENT_NOTE, C(5), E(5),
  G(5), SILENT_NOTE, G(4), SILENT_NOTE,

  C(5), G(4), SILENT_NOTE, E(4), A(4), B(4), B_FLAT(4), A(4), G(4),
  E(5), G(5), A(5), F(5), G(5), SILENT_NOTE, E(5), C(5), D(5), B(4),

  C(5), G(4), SILENT_NOTE, E(4), A(4), B(4), B_FLAT(4), A(4), G(4),
  E(5), G(5), A(5), F(5), G(5), SILENT_NOTE, E(5), C(5), D(5), B(4),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  G_SHARP(4), A(4), C(5), SILENT_NOTE, A(4), C(5), D(5),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  C(6), SILENT_NOTE, C(6), SILENT_NOTE, C(6),

  SILENT_NOTE, G(5), F_SHARP(5), F(5), D_SHARP(5), E(5), SILENT_NOTE,
  G_SHARP(4), A(4), C(5), SILENT_NOTE, A(4), C(5), D(5),

  SILENT_NOTE, E_FLAT(5), SILENT_NOTE, D(5), C(5)
};

unsigned int duration[MELODY_LENGTH] =
{
  100, 25, 125, 125, 125, 125, 125, 250, 250, 250, 250, 250,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 125, 125, 125, 250, 125, 125, 200, 50, 100, 25, 500,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 250, 125, 375, 500
};

unsigned char currentIdx;

int main()                    // run once, when the sketch starts
{
  currentIdx = 0;
  print("Music!");

  while(1)                     // run over and over again
  {
    // if we haven't finished playing the song and 
    // the buzzer is ready for the next note, play the next note
    if (currentIdx < MELODY_LENGTH && !is_playing())
    {
      // play note at max volume
      play_note(note[currentIdx], duration[currentIdx], 15);
      
      // optional LCD feedback (for fun)
      lcd_goto_xy(0, 1);                           // go to start of the second LCD line
	  if(note[currentIdx] != 255) // display blank for rests
        print_long(note[currentIdx]);  // print integer value of the current note
      print("  ");                            // overwrite any left over characters
      currentIdx++;
    }

    // Insert some other useful code here...
    // the melody will play normally while the rest of your code executes
    // as long as it executes quickly enough to keep from inserting delays
    // between the notes.
  
    // For example, let the top user pushbutton function as a stop/reset melody button
    if (button_is_pressed(TOP_BUTTON))
    {
      stop_playing(); // silence the buzzer
      if (currentIdx < MELODY_LENGTH)
        currentIdx = MELODY_LENGTH;        // terminate the melody
      else
        currentIdx = 0;                    // restart the melody
      wait_for_button_release(TOP_BUTTON);  // wait here for the button to be released
    }
  }
}
