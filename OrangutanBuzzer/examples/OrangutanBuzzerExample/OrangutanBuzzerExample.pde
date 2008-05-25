#include <OrangutanBuzzer.h>

/*
 * OrangutanBuzzerExample for the Orangutan LV-168
 *
 * This example program is indended for use on the Orangutan LV-168.
 * It uses the OrangutanBuzzer library to play a series of notes on
 * the Orangutan LV-168's buzzer.
 *
 */

#define MELODY_LENGTH 95

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

OrangutanBuzzer buzzer;
unsigned char currentIdx;

void setup()                    // run once, when the sketch starts
{
  buzzer.init();
  currentIdx = 0;
}

void loop()                     // run over and over again
{
  // if we haven't finished playing the song and 
  // the buzzer is ready for the next note, play the next note
  if (currentIdx < MELODY_LENGTH && !buzzer.isPlaying())
  {
    // play note at max volume
    buzzer.playNote(note[currentIdx], duration[currentIdx], 15);
    currentIdx++;
  }

  // Insert some other useful code here...
  // the melody will play normally while the rest of your code executes
  // as long as it executes quickly enough to keep from inserting delays
  // between the notes.
}
