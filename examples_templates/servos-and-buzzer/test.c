#include <pololu/orangutan.h>
#include <stdio.h>	// required for printf()

/*
 * servos-and-buzzer: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example demonstrates how you can have a program that makes sounds
 * and uses the OrangutanServos library.  OrangutanServos and OrangutanBuzzer
 * both use the same hardware timer (timer 1) to operate, so the two
 * libraries are not compatible with each other.  However, they can be
 * alternately used to make sound, then control servos, then make sounds
 * again.  This program also defines a simple note-playing function
 * (play_simple_note()) that uses loop delays to generate a signal with
 * the appropriate frequency on the buzzer pin.  This is a blocking function,
 * but it does not use timer 1 and does not require the OrangutanBuzzer
 * library (other than some #defines from the header file), so it can be
 * used to play notes while servos are active.
 *
 * This example uses the OrangutanServos functions to control two servos
 * connected to pins PD0 and PD1.  The servo pulses are generated in the
 * background using interrupts, so program execution is not blocked by
 * the pulse-generation code.  If you are using the Orangutan SV or SVP,
 * you can power the servos directly from the board's 5V bus.  For all
 * other Orangutans (LV, X2, Baby) and the 3pi robot, you need to power
 * the servos from a separate source since the on-board voltage regulators
 * do not have enough power to drive typical servos.  This example uses
 * pushbutton inputs to trigger events, so you will need connect an external
 * pushbutton (or wire) to the correct pin if if you are using a Baby
 * Orangutan.
 *
 * Note: the "B" version of the servo functions are used in this example
 * so that it behaves the same for the Orangutan SVP as for all other
 * Orangutans.  If you are not using the Orangutan SVP, you can do not
 * need to use the B versions of the functions (e.g. you can use
 * set_servo_target(0, 1200) instead of set_servo_target_b(0, 1200)).
 * If you do this, you should replace the call to servos_init_extended()
 * with servos_init().
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

#ifndef _ORANGUTAN_X2	// this code isn't needed if we're using the Orangutan X2

// frequencies in tenths of a Hertz of lowest 12 allowed notes (stored in program memory)
unsigned int base_freqs[] PROGMEM = {412, 437, 463, 490, 519, 550, 583, 617, 654, 693, 734, 778}; 

// returns the frequency (in Hz) for the specified note.  Note macros are
// defined in OrangutanBuzzer.h.  For example to get the frequency of note
// C sharp in octave 5:
//   get_frequency(C_SHARP(5));
unsigned int get_frequency(unsigned char note)
{
	unsigned int freq = 0;
	unsigned char offset_note = note - 16;

	if (note <= 16)
		offset_note = 0;
	else if (offset_note > 95)
		offset_note = 95;

	unsigned char exponent = offset_note / 12;

	// read the base frequency for the requested note from program memory
	freq = pgm_read_word(base_freqs + (offset_note % 12));

	// in the following calculations we add half of the denominator to the
	// numerator to get a result that is rounded properly (otherwise, the
	// decimal is just truncated and something like 3.9 becomes 3).
	if (exponent < 7)
	{
		freq = ((freq<<exponent) + 5) / 10;	// == (frequency * 2^exponent) / 10
	}
	else
		freq = (freq*64 + 2) / 5;	// == freq * 2^7 / 10 without int overflow

	return freq;
}

#endif


// Play a note without using timer 1 or the OrangutanBuzzer library, which
// means it is safe to use this function while using the OrangutanServo or
// OrangutanPulseIn libraries.
// This is a blocking function; execution does not return until note is done.
// This function will not have an effect if timer 1 is configured for
// buzzer operation.  The argument freq is in Hz and duration is in ms.
void play_simple_note(unsigned char note, unsigned int duration)
{
#ifdef _ORANGUTAN_X2	// buzzer is controlled by aux MCU on X2

	x2_play_note(note, duration);
	delay_ms(duration);

#else	// if using any non-X2 Orangutan or 3pi robot

	unsigned long time = get_ms();
	unsigned int freq = get_frequency(note);
	unsigned int half_us_delay = (500000UL + freq/2) / freq;

	while (get_ms() - time <= duration)
	{
		// BUZZER_IO is buzzer pin (defined in OrangutanBuzzer.h)
		// use loop delays to generate a square wave on buzzer pin at
		// the appropriate frequency for the note we want to play
		set_digital_output(BUZZER_IO, HIGH);
		delay_us(half_us_delay);
		set_digital_output(BUZZER_IO, LOW);
		delay_us(half_us_delay);
	}

#endif
}


int main()
{
	play_from_program_space(fugue);

	clear();
	lcd_init_printf();			// required for using printf()

	printf("Fugue");
	delay_ms(200);
	printf(".");
	delay_ms(200);
	printf(".");
	delay_ms(200);
	printf(".");

	delay_ms(500);

	while (1)
	{
		clear();
		printf("Press a\nbutton");

		wait_for_button_press(ANY_BUTTON);

		// setup pins D0 and D1 as servo outputs and initialize timer 1 for servo pulses
		servos_start_extended(0, 0, (unsigned char[]) {IO_D0, IO_D1}, 2);

		// move servos at full speed to their initial positions
		set_servo_target_b(0, 1200);
		set_servo_target_b(1, 1200);

		delay_ms(50);	// let positions be updated before we change the speed

		// set servo speed to something slower
		set_servo_speed_b(0, 200);
		set_servo_speed_b(1, 100);


		while (1)
		{
			set_servo_target_b(0, 1200);
			// blocking sound functions that works while servos are active
			play_simple_note(C(5), 50);
			play_simple_note(E(4), 50);

			set_servo_target_b(1, 1200);
			delay_ms(400);

			if (button_is_pressed(ANY_BUTTON))
			{
				wait_for_button_release(button_is_pressed(ANY_BUTTON));
				servos_stop();
				break;
			}

			set_servo_target_b(0, 1800);
			delay_ms(200);
			set_servo_target_b(1, 1800);
			delay_ms(400);

			if (button_is_pressed(ANY_BUTTON))
			{
				wait_for_button_release(button_is_pressed(ANY_BUTTON));
				// disable servo signals (allow OrangutanBuzzer code to be used again)
				servos_stop();
				break;
			}
		}

		// since we have called servos_stop(), we can use OrangutanBuzzer code again
		play("cdefgab>c");
		delay(1000);
	}
}