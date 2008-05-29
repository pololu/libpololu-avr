/*
  OrangutanBuzzer.h - Library for controlling the buzzer on the
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

#ifndef OrangutanBuzzer_h
#define OrangutanBuzzer_h


//                                             n
// Equal Tempered Scale is given by f  = f  * a
//                                   n    o
//
//  where f  is chosen as A above middle C (A4) at f  = 440 Hz
//         o                                        o
//  and a is given by the twelfth root of 2 (~1.059463094359)

// key
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


class OrangutanBuzzer
{
  public:

    // constructor
	OrangutanBuzzer();

	// Sets up timer 1 to play the desired frequency (in Hz or .1 Hz) for the
	// the desired duration (in ms). Allowed frequencies are 40 Hz to 10 kHz.
	// 'volume' controls buzzer volume, with 15 being loudest and 0 being 
	// quietest.  If the most significant bit of 'freq' is set, the frequency
	// is taken to be the value of the lower 15 bits in units of .1 Hz.
	// Otherwise the units are in Hz.
	// Note: frequency*duration/1000 must be less than 0xFFFF (65535).  This
	// means that you can't use a max duration of 65535 ms for frequencies
	// greater than 1 kHz.  For example, the max duration you can use for a
	// frequency of 10 kHz is 6553 ms.  If you use a duration longer than this,
	// you will cause an integer overflow that produces unexpected behavior.
	static void playFrequency(unsigned int freq, unsigned int duration, 
				   	   unsigned char volume);
	
	// Sets up timer 1 to play the desired note for the desired duration
	// (in ms).  'volume' controls the buzzer volume, with 15 being loudest and
	// 0 being quietest.
	// note = key + octave * 12, where 0 <= key < 12
	// example: A4 = A + 4 * 12, where A = 9 (so A4 = 57)
	// A note is converted to a frequency by the formula:
	//   Freq(n) = Freq(0) * a^n
	// where
	//   Freq(0) is chosen as A4, which is 440 Hz
	// and
	//   a = 2 ^ (1/12)
	// n is the number of notes you are away from A4.
	// if note = 16, freq = 41.2 Hz (E1 - lower limit as freq must be >40 Hz)
	// if note = 57, freq = 440 Hz (A4 - central value of ET Scale)
	// if note = 111, freq = 9.96 kHz (D#9 - upper limit, freq must be <10 kHz)
	// if note = 255, freq = 1 kHz and buzzer is silent (silent note)
	static void playNote(unsigned char note, unsigned int duration,
				  unsigned char volume);


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
	// The following plays a c major scale up and back down:
	//   play("L16 V8 cdefgab>cbagfedc");
	//
	// Here is an example from Bach:
	//   play("T240 L8 a gafaeada c+adaeafa <aa<bac#ada c#adaeaf4");
	
	static void play(const char *sequence);

	// Returns 1 if the buzzer is currently playing, otherwise it returns 0
	static unsigned char isPlaying();
	
	// Stops all sound playback immediately.
	static void stopPlaying();
	
	
  private:
	
	// initializes timer1 for buzzer control
	static void init2();
	
	// this is called by playFrequency()
	static inline void init()
	{
		static unsigned char initialized = 0;

		if (!initialized)
		{
			initialized = 1;
			init2();
		}
	}
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **