/*
  OrangutanBuzzer.cpp - Library for controlling the buzzer on the
    Orangutan LV-168. This library uses a timer1 PWM to generate the note
	frequencies and timer1 overflow interrupt to time the duration of the
	notes, so the buzzer can be playing a melody in the background while
	the rest of your code executes.
  Written by Ben Schmidel and Paul Grayson, May 23, 2008.
  http://www.pololu.com
  http://forum.pololu.com
  Released into the public domain.
  
  Disclaimer: To the extent permitted by law, Pololu Corporation provides
  this work without any warranty.  It may be defective, in which case you
  agree to be responsible for all resulting costs and damages.
*/

#ifndef F_CPU
#define F_CPU 20000000UL	// Orangutans run at 20 MHz
#endif //!F_CPU

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OrangutanBuzzer.h"

#define TIMER1_OFF					0x00	// timer1 disconnected
#define TIMER1_CLK_1				0x01	// 20 MHz
#define TIMER1_CLK_8				0x02	// 2.5 MHz

#define ENABLE_TIMER1_INTERRUPT()	TIMSK1 = (1 << TOIE1)
#define DISABLE_TIMER1_INTERRUPT()	TIMSK1 = 0


// declaring these globals as static means they won't conflict
// with globals in other .cpp files that share the same name
static volatile unsigned int buzzerTimeout = 0;		// tracks buzzer time limit
static volatile unsigned char buzzerFinished = 1;	// flag: 0 while playing
static const char *sequence = 0;

static unsigned char octave;				// the current octave

static unsigned int whole_note_duration;	// the duration of a whole note
static unsigned int duration;				// the duration of a note in ms
static unsigned int volume;				// the note volume
static unsigned char staccato; 			// true if playing staccato
static unsigned char staccato_rest_duration;	// duration of a staccato
												//  rest, or zero if it is time
												//  to play a note

void nextNote();

// Timer1 overflow interrupt
ISR (TIMER1_OVF_vect)
{
	if (buzzerTimeout-- == 0)
	{
		TCCR1B = (TCCR1B & 0xF8) | TIMER1_CLK_1;	// select IO clock
		OCR1A = F_CPU / 1000;			// set TOP for freq = 1 kHz
		OCR1B = 0;						// 0% duty cycle
		DDRB &= ~(1 << PB2);	// silence buz, pin->input
		buzzerFinished = 1;
		if(sequence)
			nextNote();
	}
}


// constructor

OrangutanBuzzer::OrangutanBuzzer()
{
}

#ifdef LIB_POLOLU

extern "C" void play_frequency(unsigned int freq, unsigned int dur, 
							   unsigned char volume)
{
	OrangutanBuzzer::playFrequency(freq, dur, volume);
}

extern "C" void play_note(unsigned char note, unsigned int dur,
						  unsigned char volume)
{
	OrangutanBuzzer::playNote(note, dur, volume);
}

extern "C" void play(char *sequence)
{
	OrangutanBuzzer::play(sequence);
}

extern "C" unsigned char is_playing()
{
	return OrangutanBuzzer::isPlaying();
}

extern "C" void stop_playing()
{
	OrangutanBuzzer::stopPlaying();
}

#endif

// initializes timer1 for buzzer control
void OrangutanBuzzer::init2()
{
	DDRB &= ~(1 << PB2);		// buzzer pin set as input

	TCCR1A = 0x23;	// bits 6 and 7 clear: normal port op., OC1A disconnected
					// bit 4 clear, 5 set: clear OC1B on compare match
					// bits 2 and 3: not used
					// bits 0 & 1 set: combine with bits 3 & 4 of TCCR1B...

	TCCR1B = 0x19;	// bit 7 clear: input capture noise canceler disabled
					// bit 6 clear: input capture triggers on falling edge
					// bit 5: not used
					// bits 3 and 4 set: combine with bits 0 & 1 of TCCR1A to
					// 		select waveform generation mode 14, fast PWM,
					//		TOP = OCR1A, OCR1B set at TOP, TOV1 flag set at TOP
					// bit 0 set, 1-2 clear: timer clock = IO clk (prescaler 1)

	TCCR1C = 0x00;	// bit 7 clear: no force output compare for channel A
					// bit 6 clear: no force output compare for channel B
					// bits 0 - 5: not used

	// This sets timer 1 to run in fast PWM mode, where TOP = ICR1, 
	//   OCR1A is updated at TOP, TOV1 Flag is set on TOP.  OC1A is cleared
	//   on compare match, set at TOP; OC1B is disconnected.
	//   Note: if the PWM frequency and duty cycle are changed, the first
	//   cycle of the new frequency will be at the old duty cycle, since
	//   the duty cycle (OCR1A) is not updated until TOP.

	OCR1A = F_CPU / 1000;					// set TOP for freq = 1 kHz
	OCR1B = 0;								// set 0% duty cycle
	TCNT1 = 0;								// clear counter register

	TIFR1 = 0xFF;							// clear all timer1 interrupt flags
	ENABLE_TIMER1_INTERRUPT();				// overflow interrupt enabled
											//  all other timer1 ints disabled
}


// Set up timer 1 to play the desired frequency (in Hz or .1 Hz) for the
//   the desired duration (in ms). Allowed frequencies are 40 Hz to 10 kHz.
//   volume controls buzzer volume, with 15 being loudest and 0 being quietest.
// Note: frequency*duration/1000 must be less than 0xFFFF (65535).  This
//   means that you can't use a max duration of 65535 ms for frequencies
//   greater than 1 kHz.  For example, the max duration you can use for a
//   frequency of 10 kHz is 6553 ms.  If you use a duration longer than this,
//   you will cause an integer overflow that produces unexpected behavior.
void OrangutanBuzzer::playFrequency(unsigned int freq, unsigned int dur, 
				   					unsigned char volume)
{
	unsigned int newOCR1A;
	unsigned int newTCCR1B;
	unsigned int timeout;
	unsigned char multiplier = 1;
	
	init();		// initializes the buzzer if necessary

	buzzerFinished = 0;

	if (freq & DIV_BY_10)		// if frequency's DIV_BY_10 bit is set
	{							//   then the true frequency is freq/10
		multiplier = 10;		//   (gives higher resolution for small freqs)
		freq &= ~DIV_BY_10;		// clear DIV_BY_10 bit
	}

	newTCCR1B = TCCR1B & 0xF8;	// clear clock select bits

	// calculate necessary clock source and counter top value to get freq
	if (freq > 400 * ((unsigned int)multiplier))	// clock prescaler = 1
	{
		if (freq > 10000)
			freq = 10000;			// max frequency allowed is 10kHz

		newOCR1A = (unsigned int)(20000000UL / freq);

		// timer1 clock select:
		newTCCR1B |= TIMER1_CLK_1;	// select IO clk (prescaler = 1)
	}

	else											// clock prescaler = 8
	{
		unsigned char val = 40 * multiplier;
		if (freq < val)				// min frequency allowed is 40 Hz
			freq = val;

		// set top (frequency):
		if (multiplier == 10)
			newOCR1A = (unsigned int)(25000000UL / freq);
		else
			newOCR1A = (unsigned int)(2500000UL / freq);

		// timer1 clock select
		newTCCR1B |= TIMER1_CLK_8;	// select IO clk / 8
	}


	// set timeout (duration):
	if (multiplier == 10)
		freq = (freq + 5) / 10;

	if (freq == 1000)
		timeout = dur;	// duration for silent notes is exact
	else
		timeout = (unsigned int)((long)dur * freq / 1000);

	if (volume == 0)
		DDRB &= ~(1 << PB2);		// buzzer pin->input (silence buz.)
	else
		DDRB |= 1 << PB2;			// buzzer pin->output

	if (volume > 15)
		volume = 15;

	DISABLE_TIMER1_INTERRUPT();			// disable interrupts while writing to 16-bit registers
	TCCR1B = newTCCR1B;					// select timer 1 clock prescaler
	OCR1A = newOCR1A;					// set timer 1 pwm frequency
	OCR1B = OCR1A >> (16 - volume);		// set duty cycle (volume)
	buzzerTimeout = timeout;			// set buzzer duration
	ENABLE_TIMER1_INTERRUPT();
	sei();
}



// Determine the frequency for the specified note, then play that note
//   for the desired duration (in ms).  This is done without using floats
//   and without having to loop.  volume controls buzzer volume, with 15 being
//   loudest and 0 being quietest.
// Note: frequency*duration/1000 must be less than 0xFFFF (65535).  This
//   means that you can't use a max duration of 65535 ms for frequencies
//   greater than 1 kHz.  For example, the max duration you can use for a
//   frequency of 10 kHz is 6553 ms.  If you use a duration longer than this,
//   you will cause an integer overflow that produces unexpected behavior.
void OrangutanBuzzer::playNote(unsigned char note, unsigned int dur,
							   unsigned char volume)
{
	// note = key + octave * 12, where 0 <= key < 12
	// example: A4 = A + 4 * 12, where A = 9 (so A4 = 57)
	// A note is converted to a frequency by the formula:
	//   Freq(n) = Freq(0) * a^n
	// where
	//   Freq(0) is chosen as A4, which is 440 Hz
	// and
	//   a = 2 ^ (1/12)
	// n is the number of notes you are away from A4.
	// One can see that the frequency will double every 12 notes.
	// This function exploits this property by defining the frequencies of the
	// 12 lowest notes allowed and then doubling the appropriate frequency
	// the appropriate number of times to get the frequency for the specified
	// note.

	// if note = 16, freq = 41.2 Hz (E1 - lower limit as freq must be >40 Hz)
	// if note = 57, freq = 440 Hz (A4 - central value of ET Scale)
	// if note = 111, freq = 9.96 kHz (D#9 - upper limit, freq must be <10 kHz)
	// if note = 255, freq = 1 kHz and buzzer is silent (silent note)

	// The most significant bit of freq is the "divide by 10" bit.  If set,
	// the units for frequency are .1 Hz, not Hz, and freq must be divided
	// by 10 to get the true frequency in Hz.  This allows for an extra digit
	// of resolution for low frequencies without the need for using floats.

	unsigned int freq = 0;
	unsigned char offset_note = note - 16;

	if (note == SILENT_NOTE || volume == 0)
	{
		freq = 1000;	// silent notes => use 1kHz freq (for cycle counter)
		playFrequency(freq, dur, 0);
		return;
	}

	if (note <= 16)
		offset_note = 0;
	else if (offset_note > 95)
		offset_note = 95;

	unsigned char exponent = offset_note / 12;

	// frequency table for the lowest 12 allowed notes
	//   frequencies are specified in tenths of a Hertz for added resolution
	switch (offset_note - exponent * 12)	// equivalent to (offset_note % 12)
	{
		case 0:				// note E1 = 41.2 Hz
			freq = 412;
			break;
		case 1:				// note F1 = 43.7 Hz
			freq = 437;
			break;
		case 2:				// note F#1 = 46.3 Hz
			freq = 463;
			break;
		case 3:				// note G1 = 49.0 Hz
			freq = 490;
			break;
		case 4:				// note G#1 = 51.9 Hz
			freq = 519;
			break;
		case 5:				// note A1 = 55.0 Hz
			freq = 550;
			break;
		case 6:				// note A#1 = 58.3 Hz
			freq = 583;
			break;
		case 7:				// note B1 = 61.7 Hz
			freq = 617;
			break;
		case 8:				// note C2 = 65.4 Hz
			freq = 654;
			break;
		case 9:				// note C#2 = 69.3 Hz
			freq = 693;
			break;
		case 10:			// note D2 = 73.4 Hz
			freq = 734;
			break;
		case 11:			// note D#2 = 77.8 Hz
			freq = 778;
			break;
	}

	if (exponent < 7)
	{
		freq = freq << exponent;	// frequency *= 2 ^ exponent
		if (exponent > 1)			// if the frequency is greater than 160 Hz
			freq = (freq + 5) / 10;	//   we don't need the extra resolution
		else
			freq += DIV_BY_10;		// else keep the added digit of resolution
	}
	else
		freq = (freq * 64 + 2) / 5;	// == freq * 2^7 / 10 without int overflow

	if (volume > 15)
		volume = 15;
	playFrequency(freq, dur, volume);	// set buzzer this freq/duration
}


// Returns 1 if the buzzer is currently playing, otherwise it returns 0
unsigned char OrangutanBuzzer::isPlaying()
{
	return !buzzerFinished;
}


// Plays the specified sequence of notes without requiring any further
// input from the user.  Modeled after the PLAY commands in
// GW-BASIC, with just a few differences.
//
// The notes are specified by the characters C, D, E, F, G, A, and
// B, and they are played by default as "quarter notes" with a
// length of 500 ms.  This corresponds to a tempo of 120
// beats/min.  The special note R plays a rest (no sound).
//
// Various control characters alter the sound:
//   '>' plays the next note one octave higher
//
//   '<' plays the next note one octave lower
//
//   '+' or '#' after a note raises any note one half-step
//
//   '-' after a note lowers any note one half-step
//
//   '.' after a note "dots" it, increasing the length by
//       50%.  Each additional dot adds half as much as the
//       previous dot, so that "A.." is 1.75 times the length of
//       "A".
//
//   'O' followed by a number sets the octave (default: O4).
//
//   'T' followed by a number sets the tempo (default: T120).
//
//   'L' followed by a number sets the default note duration to
//       the type specified by the number: 4 for quarter notes, 8
//       for eighth notes, 16 for sixteenth notes, etc.
//
//   'V' followed by a number from 1-15 sets the music volume.
//
// The following plays a quiet c major scale up and back down:
//   play("V8 cdefgab>cbagfedc");
//
// Here is an example from Bach:
//   play("T240 L8 a gafaeada c+adaeafa <aa<bac#ada c#adaeaf4");
void OrangutanBuzzer::play(const char *notes)
{
	DISABLE_TIMER1_INTERRUPT();	// prevent this from being interrupted
	octave = 4; // the current octave
	whole_note_duration = 2000; // the whole note duration
	duration = 500; // the duration of a note in ms
	volume = 15; // the note volume
	staccato = 0;
	sequence = notes;
	nextNote();
	ENABLE_TIMER1_INTERRUPT();	// re-enable interrupts
}


// stop all sound playback immediately
void OrangutanBuzzer::stopPlaying()
{
	DISABLE_TIMER1_INTERRUPT();					// diable interrupts
	TCCR1B = (TCCR1B & 0xF8) | TIMER1_CLK_1;	// select IO clock
	OCR1A = F_CPU / 1000;						// set TOP for freq = 1 kHz
	OCR1B = 0;									// 0% duty cycle
	DDRB &= ~(1 << PB2);						// silence buz, pin->input
	buzzerFinished = 1;
	sequence = 0;
	ENABLE_TIMER1_INTERRUPT();					// re-enable interrupts
}

// Returns the numerical argument specified at sequence[0] and
// increments sequence to point to the character immediately after the
// argument.
unsigned int getNumber()
{
	unsigned int arg = 0;

	// read all digits, one at a time
	while(*sequence >= '0' && *sequence <= '9')
	{
		arg *= 10;
		arg += *sequence-'0';
		sequence ++;
	}

	return arg;
}

// Returns whole_note_duration/getNumber()
unsigned int getDuration()
{
	return whole_note_duration/getNumber();
}

void nextNote()
{
	unsigned char note = 0;
	unsigned char rest = 0;
	unsigned char tmp_octave = octave; // the octave for this note
	unsigned int tmp_duration; // the duration of this note
	unsigned int dot_add;

	char c; // temporary variable

	// if we are playing staccato, after every note we play a rest
	if(staccato && staccato_rest_duration)
	{
		OrangutanBuzzer::playNote(20, staccato_rest_duration, 0);
		staccato_rest_duration = 0;
		return;
	}

 parse_character:

	// Convert the current character to lower case.
	c = *sequence;
	if(c >= 'A' && c <= 'Z')
		c += 'a'-'A';

	// Advance to the next character.
	sequence ++;

	// Interpret the character.
	switch(c)
	{
	case ' ':
		// ignore spaces
		goto parse_character;
	case '>':
		// shift the octave temporarily up
		tmp_octave ++;
		goto parse_character;
	case '<':
		// shift the octave temporarily down
		tmp_octave --;
		goto parse_character;
	case 'a':
		note = A(0);
		break;
	case 'b':
		note = B(0);
		break;
	case 'c':
		note = C(0);
		break;
	case 'd':
		note = D(0);
		break;
	case 'e':
		note = E(0);
		break;
	case 'f':
		note = F(0);
		break;
	case 'g':
		note = G(0);
		break;
	case 'l':
		// set the default note duration
		duration = getDuration();
		goto parse_character;
	case 'm':
		// set music staccato or legato
		if(sequence[0] == 'l' || sequence[0] == 'L')
			staccato = false;
		else
		{
			staccato = true;
			staccato_rest_duration = 0;
		}
		sequence++;
		goto parse_character;
	case 'o':
		// set the octave permanently
		octave = getNumber();
		tmp_octave = octave;
		goto parse_character;
	case 'r':
		// Rest - the note value doesn't matter.
		rest = 1;
		break;
	case 't':
		// set the tempo
		whole_note_duration = 60*400/getNumber()*10;
		duration = whole_note_duration/4;
		goto parse_character;
	case 'v':
		// set the volume
		volume = getNumber();
		goto parse_character;
	default:
		sequence = 0;
		return;
	}

	note += tmp_octave*12;

	// handle sharps and flats
	while(*sequence == '+' || *sequence == '#')
	{
		sequence ++;
		note ++;
	}
	while(*sequence == '-')
	{
		sequence ++;
		note --;
	}

	// set the duration of just this note
	tmp_duration = duration;

	// If the input is 'c16', make it a 16th note, etc.
	if(*sequence > '0' && *sequence < '9')
		tmp_duration = getDuration();

	// Handle dotted notes - the first dot adds 50%, and each
	// additional dot adds 50% of the previous dot.
	dot_add = tmp_duration/2;
	while(*sequence == '.')
	{
		sequence ++;
		tmp_duration += dot_add;
		dot_add /= 2;
	}

	if(staccato)
	{
		staccato_rest_duration = tmp_duration / 2;
		tmp_duration -= staccato_rest_duration;
	}
	OrangutanBuzzer::playNote(note, tmp_duration, rest ? 0 : volume);
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
