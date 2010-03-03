/*
  OrangutanX2.h - Library of functions for controlling the auxiliary
	processor on the Orangutan X2.
*/

/*
 * Written by Ben Schmidel, Mar 2, 2010.
 * Copyright (c) 2010 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the three links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */

#ifndef OrangutanX2_h
#define OrangutanX2_h

#include "../OrangutanResources/include/OrangutanModel.h"

#ifdef _ORANGUTAN_X2

#define MOTOR1		0
#define MOTOR2		1
#define JOINT_MOTOR	0xFF

#define IMMEDIATE_DRIVE	0
#define ACCEL_DRIVE	1
#define BRAKE_LOW		0xFF

#ifdef __cplusplus

// C++ Function Declarations

class OrangutanX2
{
  public:
	static unsigned char getStatus();
	static void getFirmwareVersion(unsigned char &majorVersion, unsigned char &minorVersion);
	static void setMotor(unsigned char motor, unsigned char operationMode, int speed);
	static void playNote(unsigned char note, unsigned int duration);
	static void playFrequency(unsigned int frequency, unsigned int duration);
	static void buzzerOff();
	static void setVolume(unsigned char volume);
	static void setNoteGap(unsigned char noteGap);
};

#else

// C Function Declarations

void x2_get_firmware_version(unsigned char *majorVersion, unsigned char *minorVersion);
unsigned char x2_get_status();
void x2_set_motor(unsigned char motor, unsigned char operationMode, int speed);
void x2_play_note(unsigned char note, unsigned int duration);
void x2_play_frequency(unsigned int frequency, unsigned int duration);
void x2_buzzer_off();
void x2_set_volume(unsigned char volume);
void x2_set_note_gap(unsigned char noteGap);

#endif // _cplusplus

#endif // _ORANGUTAN_X2

#endif // OrangutanX2_h
