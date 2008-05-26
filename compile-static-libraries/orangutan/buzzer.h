/*
  OrangutanBuzzer.h - Library for controlling the buzzer on the
      Orangutan LV-168
  Written by Ben Schmidel, May 23, 2008.
  Released into the public domain.

  This library uses a timer1 PWM to generate the note frequencies and
  timer1 overflow interrupt to time the duration of the notes,
  so the buzzer can be playing a melody in the background while the rest
  of your code executes.
*/
#ifndef OrangutanBuzzer_h
#define OrangutanBuzzer_h

void buzzer_init();
void play_frequency(unsigned int freq, unsigned int duration, 
		       unsigned char volume);
void play_note(unsigned char note, unsigned int duration,
		  unsigned char volume);
void play(char *sequence);
unsigned char is_playing();

#endif
