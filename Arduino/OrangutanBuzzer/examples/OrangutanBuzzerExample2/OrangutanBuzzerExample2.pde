#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

/*
 * OrangutanBuzzerExample2 for the Orangutan LV-168
 *
 * This example program is indended for use on the Orangutan LV-168.
 * It uses the OrangutanBuzzer library to play a series of notes on
 * the Orangutan LV-168's buzzer.  It uses the OrangutanPushbuttons
 * library to allow the user select which melody plays.
 *
 * This example demonstrates the use of the OrangutanBuzzer::play() method,
 * which plays the specified melody entirely in the background, requiring
 * no further action from the user once the method is called.  The CPU
 * is then free to execute other code while the melody plays.
 *
 * http://www.pololu.com/docs/0J17/5.b
 * http://www.pololu.com
 * http://forum.pololu.com
 */

OrangutanLCD lcd;
OrangutanPushbuttons buttons;
OrangutanBuzzer buzzer;

void setup()                    // run once, when the sketch starts
{
  lcd.print("Press a");
  lcd.gotoXY(0, 1);
  lcd.print("button..");
}

void loop()                     // run over and over again
{
  // wait here for one of the three buttons to be pushed
  unsigned char button = 
    buttons.waitForButton(TOP_BUTTON | MIDDLE_BUTTON | BOTTOM_BUTTON);
  lcd.clear();
  
  if (button == TOP_BUTTON)
  {
    buzzer.play("T120 V15 L16" 
       "agafaea dac+adaea fa<aa<bac#a dac#adaea f"
       "O5 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad "
       "O5 L8 MS <b-d<b-d ML e-<ge-<g MS c<ac<a ML d<fd<f O4 MS b-gb-g ML >c#e>c#e"
       "MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
       "L16 O5 ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca <b-acadgb-g egdgcg<b-g"
       "<ag<b-gcf<af dfcf<b-f<af <gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
       "O4 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d e>d>c#>db>d>c#b"
       ">c#agaegfe fO5dc#dfdc#<b c#4"
    );

    lcd.print("Fugue!");
  }
  if (button == MIDDLE_BUTTON)
  {
    buzzer.play("V8 cdefgab>cbagfedc");
    lcd.print("C Major");
  }
  if (button == BOTTOM_BUTTON && buzzer.isPlaying())
  {
    buzzer.stopPlaying();
    lcd.print("stopped"); 
  }
}
