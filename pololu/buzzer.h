/*
  OrangutanBuzzer.h - Library for controlling the buzzer on the
      Orangutan LV, SV, SVP, or 3pi robot.
  Written by Ben Schmidel, May 23, 2008.

  This library uses a timer1 PWM to generate the note frequencies and
  timer1 overflow interrupt to time the duration of the notes,
  so the buzzer can be playing a melody in the background while the rest
  of your code executes.
*/
#ifndef OrangutanBuzzer_h
#define OrangutanBuzzer_h

#define PLAY_AUTOMATIC 0
#define PLAY_CHECK 1

#define C(x)			( 0 + (x)*12)
#define C_SHARP(x)		( 1 + (x)*12)
#define D_FLAT(x)		( 1 + (x)*12)
#define D(x)			( 2 + (x)*12)
#define D_SHARP(x)		( 3 + (x)*12)
#define E_FLAT(x)		( 3 + (x)*12)
#define E(x)			( 4 + (x)*12)
#define F(x)			( 5 + (x)*12)
#define F_SHARP(x)		( 6 + (x)*12)
#define G_FLAT(x)		( 6 + (x)*12)
#define G(x)			( 7 + (x)*12)
#define G_SHARP(x)		( 8 + (x)*12)
#define A_FLAT(x)		( 8 + (x)*12)
#define A(x)			( 9 + (x)*12)
#define A_SHARP(x)		(10 + (x)*12)
#define B_FLAT(x)		(10 + (x)*12)
#define B(x)			(11 + (x)*12)

// special notes
#define A4				A(4)			// center of the Equal-Tempered Scale
#define SILENT_NOTE		0xFF			// this note will silence the buzzer

#define DIV_BY_10		(1 << 15)		// frequency bit that indicates Hz/10

void buzzer_init();
void play_frequency(unsigned int freq, unsigned int duration, 
		       unsigned char volume);
void play_note(unsigned char note, unsigned int duration,
		  unsigned char volume);
void play(const char *sequence);
void play_from_program_space(const char *sequence);
unsigned char is_playing();
void stop_playing();

unsigned char play_check();
void play_mode(char on);

#endif
