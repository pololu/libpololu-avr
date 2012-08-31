/*
 * buzzer3: for the Orangutan LV, SV, SVP, X2, and 3pi robot.
 *
 * This example program is indended for use on the Orangutan LV, SV,
 * SVP, X2, and 3pi robot and will NOT work under the Arduino environment.
 * It uses the OrangutanBuzzer functions to play a series of notes on
 * the Orangutan's buzzer.
 *
 * This example demonstrates the use of the play_mode()
 * and play_check() methods, which allow you to select
 * whether the melody sequence initiated by play() is
 * played automatically in the background by the Timer1 interrupt, or if
 * the play is driven by the play_check() method in your main loop.
 *
 * Automatic play mode should be used if your code has a lot of delays
 * and is not time critical.  In this example, automatic mode is used
 * to allow the melody to keep playing while we blink the red user LED.
 *
 * Play-check mode should be used during parts of your code that are
 * time critical.  In automatic mode, the Timer1 interrupt is very slow
 * when it loads the next note, and this can delay the execution of your.
 * Using play-check mode allows you to control when the next note is
 * loaded so that it doesn't occur in the middle of some time-sensitive
 * measurement.  In our example we use play-check mode to keep the melody
 * going while performing timing measurements using Timer2.  After the
 * measurements, the maximum time measured is displayed on the LCD.
 *
 * Immediately below are three #define statements that allow you to alter
 * the way this program runs.  You should have one of the three lines
 * uncommented while commenting out the other two:  
 *
 * If only WORKING_CORRECTLY is uncommented, the program should run in its
 * ideal state, using automatic play mode during the LED-blinking phase
 * and using play-check mode during the timing phase.  The maximum recorded
 * time should be 20, as expected.
 *
 * If only ALWAYS_AUTOMATIC is uncommented, the program will use automatic
 * play mode during both the LED-blinking phase and the timing phase.  Here
 * you will see the effect this has on the time measurements (instead of 20,
 * you should see a maximum reading of around 27 or 28).
 *
 * If only ALWAYS_CHECK is uncommented, the program will be in play-check
 * mode during both the LED-blinking phase and the timing phase.  Here you
 * will see the effect that the LED-blinking delays have on play-check
 * mode (the sequence will be very choppy while the LED is blinking, but
 * sound normal during the timing phase).  The maximum timing reading should
 * be 20, as expected.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

#include <pololu/orangutan.h>
 
// *** UNCOMMENT ONE OF THE FOLLOWING PREPROCESSOR DIRECTIVES ***
// (the remaining two should be commented out)
#define WORKING_CORRECTLY  // this is the right way to use playMode()
//#define ALWAYS_AUTOMATIC // mode is always PLAY_AUTOMATIC, timing is inaccurate
//#define ALWAYS_CHECK     // mode is always PLAY_CHECK, delays interrupt the sequence

#include <avr/pgmspace.h>
const char rhapsody[] PROGMEM = "O6 T40 L16 d#<b<f#<d#<f#<bd#f#"
  "T80 c#<b-<f#<c#<f#<b-c#8"
  "T180 d#b<f#d#f#>bd#f#c#b-<f#c#f#>b-c#8 c>c#<c#>c#<b>c#<c#>c#c>c#<c#>c#<b>c#<c#>c#"
  "c>c#<c#>c#<b->c#<c#>c#c>c#<c#>c#<b->c#<c#>c#"
  "c>c#<c#>c#f>c#<c#>c#c>c#<c#>c#f>c#<c#>c#"
  "c>c#<c#>c#f#>c#<c#>c#c>c#<c#>c#f#>c#<c#>c#d#bb-bd#bf#d#c#b-ab-c#b-f#d#";


int main()
{
  TCCR0A = 0;         // configure timer0 to run at 78 kHz
  TCCR0B = 0x04;      // and overflow when TCNT0 = 256 (~3 ms)
  play_from_program_space(rhapsody);

  while(1)
  {
    // allow the sequence to keep playing automatically through the following delays
#ifndef ALWAYS_CHECK
    play_mode(PLAY_AUTOMATIC);
#else
    play_mode(PLAY_CHECK);
#endif
    lcd_goto_xy(0, 0);
    print("blink!");
    int i;
    for (i = 0; i < 8; i++)
    {
#ifdef ALWAYS_CHECK
      play_check();
#endif
      red_led(1);
      delay_ms(500);
      red_led(0);
      delay_ms(500);
    }
  
    lcd_goto_xy(0, 0);
    print("timing");
    lcd_goto_xy(0, 1);
    print("        ");    // clear bottom LCD line
    // turn off automatic playing so that our time-critical code won't be interrupted by
    // the buzzer's long timer1 interrupt.  Otherwise, this interrupt could throw off our
    // timing measurements.  Instead, we will now use playCheck() to keep the sequence
    // playing in a way that won't throw off our measurements.
#ifndef ALWAYS_AUTOMATIC
    play_mode(PLAY_CHECK);
#endif
    unsigned char maxTime = 0;
    for (i = 0; i < 8000; i++)
    {
      TCNT0 = 0;
      while (TCNT0 < 20)    // time for ~250 us
        ;
      if (TCNT0 > maxTime)
        maxTime = TCNT0;    // if the elapsed time is greater than the previous max, save it
#ifndef ALWAYS_AUTOMATIC
      play_check();   // check if it's time to play the next note and play it if so
#endif
    }
    lcd_goto_xy(0, 1);
    print("max=");
    print_long((unsigned int)maxTime);
    print(" ");  // overwrite any left over characters
  }
}
