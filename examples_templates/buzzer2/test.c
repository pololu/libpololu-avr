#include <pololu/orangutan.h>

/*
 * buzzer2: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * Uses the OrangutanBuzzer functions to play a series of notes on the
 * Orangutan's buzzer.  It uses the OrangutanPushbuttons library to
 * allow the user select which melody plays.
 *
 * This example demonstrates the use of the play() method,
 * which plays the specified melody entirely in the background, requiring
 * no further action from the user once the method is called.  The CPU
 * is then free to execute other code while the melody plays.
 *
 * This code will work on the Baby Orangutan if you connect a buzzer
 * to pin PB2.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#include <avr/pgmspace.h>  // this lets us refer to data in program space (i.e. flash)
// store this fugue in program space using the PROGMEM macro.  
// Later we will play it directly from program space, bypassing the need to load it 
// all into RAM first.
const char fugue[] PROGMEM = 
  "! O5 L16 agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8 MS <b-d<b-d MLe-<ge-<g MSc<ac<a ML d<fd<f O5 MS b-gb-g"
  "ML >c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6 L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe f O6 dc#dfdc#<b c#4";

void loop()                     // run over and over again
{
  // wait here for one of the three buttons to be pushed
  unsigned char button = wait_for_button(ANY_BUTTON);
  clear();
  
  if (button == TOP_BUTTON)
  {
    play_from_program_space(fugue);

    print("Fugue!");
    lcd_goto_xy(0, 1);
    print("flash ->");
  }
  if (button == MIDDLE_BUTTON)
  {
    play("! V8 cdefgab>cbagfedc");
    print("C Major");
    lcd_goto_xy(0, 1);
    print("RAM ->");
  }
  if (button == BOTTOM_BUTTON)
  {
    if (is_playing())
    {
      stop_playing();
      print("stopped");
    }
    else
    {
      play_note(A(5), 200, 15);
      print("note A5"); 
    }
  }
}

int main()                    // run once, when the program starts
{
  print("Press a");
  lcd_goto_xy(0, 1);
  print("button..");

  while(1)
  	loop();
}
