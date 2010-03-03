/*
  OrangutanX2.cpp - Library of functions for controlling the auxiliary
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

#include "../OrangutanResources/include/OrangutanModel.h"
#include "../OrangutanSPIMaster/OrangutanSPIMaster.h"
#include "../OrangutanTime/OrangutanTime.h"
#include "OrangutanX2.h"

#ifdef _ORANGUTAN_X2

#ifdef LIB_POLOLU

extern "C" void x2_get_firmware_version(unsigned char *majorVersion, unsigned char *minorVersion)
{
	OrangutanX2::getFirmwareVersion(*majorVersion, *minorVersion);
}

extern "C" unsigned char x2_get_status()
{
	return OrangutanX2::getStatus();
}

extern "C" void x2_set_motor(unsigned char motor, unsigned char operationMode, int speed)
{
	OrangutanX2::setMotor(motor, operationMode, speed);
}

extern "C" void x2_play_note(unsigned char note, unsigned int duration)
{
	OrangutanX2::playNote(note, duration);
}

extern "C" void x2_play_frequency(unsigned int frequency, unsigned int duration)
{
	OrangutanX2::playFrequency(frequency, duration);
}

extern "C" void x2_buzzer_off()
{
	OrangutanX2::buzzerOff();
}

extern "C" void x2_set_volume(unsigned char volume)
{
	OrangutanX2::setVolume(volume);
}

extern "C" void x2_set_note_gap(unsigned char noteGap)
{
	OrangutanX2::setNoteGap(noteGap);
}


#endif



void OrangutanX2::getFirmwareVersion(unsigned char &majorVersion, unsigned char &minorVersion)
{
	OrangutanSPIMaster::transmitAndDelay(253, 3);
	majorVersion = OrangutanSPIMaster::transmitAndDelay(0, 3);
	minorVersion = OrangutanSPIMaster::transmit(0);
}

// Most bits of the status byte are latched once set.  Calling this function returns the status
// byte and clears any latched status bits.  The status bits are enumerated as follows:
//   bit 0 = UART error (*LATCHED* when set, cleared when read)
//   bit 1 = UART read ready (value always reflects current state)
//   bit 2 = UART send buffer full (value always reflects current state)
//   bit 3 = buzzer finished (value always reflects current state)
//   bit 4 = motor 1 fault (*LATCHED* when set, cleared when read)
//   bit 5 = motor 1 overcurrent (*LATCHED* when set, cleared when read)
//   bit 6 = motor 2 fault (*LATCHED* when set, cleared when read)
//   bit 7 = motor 2 overcurrent (*LATCHED* when set, cleared when read)
unsigned char OrangutanX2::getStatus()
{
	OrangutanSPIMaster::transmitAndDelay(218, 3);
	return OrangutanSPIMaster::transmit(0);
}



// motor: 0 for M1, 1 for M2, 0xFF for joint
// operationMode: 0 for immediate drive, 1 for acceleration drive, 0xFF for brake low
// speed: -255 to 255 (absolute value of speed used when braking, otherwise sign = direction)
void OrangutanX2::setMotor(unsigned char motor, unsigned char operationMode, int speed)
{
	static unsigned char inJointMotorMode = 0;

	if (motor == JOINT_MOTOR && !inJointMotorMode)
	{
		OrangutanSPIMaster::transmit(215);	// enable joint-motor mode
		inJointMotorMode = 1;
	}
	if (motor != JOINT_MOTOR && inJointMotorMode)
	{
		OrangutanSPIMaster::transmit(214);	// disable joint-motor mode
		inJointMotorMode = 0;
	}
	unsigned char cmd;

	if (motor == JOINT_MOTOR)				// if joint motor command
	{
		if (operationMode == ACCEL_DRIVE)	// if accel
			cmd = 228;				// joint motor accel forward
		else if (operationMode == BRAKE_LOW)	// if brake
			cmd = 144;				// joint motor brake low
		else						// else immediate drive
			cmd = 146;				// joint motor forward
	}
	else
	{
		if (motor != MOTOR2)
			motor = MOTOR1;
		if (operationMode == ACCEL_DRIVE)	// if accel
			cmd = 232 + motor * 4;		// motor accel forward
		else if (operationMode == BRAKE_LOW)	// if brake
			cmd = 128 + motor * 4;		// motor brake low
		else						// else immediate drive
			cmd = 136 + motor * 4;		// motor forward
	}

	if (speed < 0)
	{
		speed = -speed;	// make speed a positive quantity
		if (operationMode != BRAKE_LOW)	// if not braking
			cmd += 2;				// turn command from forward to reverse
	}
	if (speed > 255)
		speed = 255;
	
	// set LSB of command byte if speed has MSB set (MSB of speed is sent as LSB of cmd)
	OrangutanSPIMaster::transmit(((unsigned char)speed) & 0x80 ? cmd | 1 : cmd);
	// data byte is 7 low bits of speed
	OrangutanSPIMaster::transmit(((unsigned char)speed) & 0x7F);
}


// Silence the buzzer immediately
void OrangutanX2::buzzerOff()
{
	OrangutanSPIMaster::transmit(225);
}


// Play the specified note for the specified duration (in ms) immediately.
// Note enumeration is provided as a set of #define macros in SPI.h.
void OrangutanX2::playNote(unsigned char note, unsigned int duration)
{
	unsigned char *byte_ptr = (unsigned char*)&duration;
	unsigned char lo = *byte_ptr;		// low byte of duration
	unsigned char hi = *(byte_ptr + 1);	// high byte of duration

	// insert the MSBs of the three data bytes into the command byte
	OrangutanSPIMaster::transmit( 152 | ((lo & 0x80) >> 5) | ((hi & 0x80) >> 6)
							   | ((note & 0x80) >> 7));
	OrangutanSPIMaster::transmit(note & 0x7F);
	OrangutanSPIMaster::transmit(hi & 0x7F);
	OrangutanSPIMaster::transmit(lo & 0x7F);
}


// Play the specified frequency (in Hz) for the specified duration (in ms)
// immediately.  Frequency must be no greater than 0x7FFF (15-bit value).
// The mega168 can only play frequencies as low as 40Hz and as high as
// 10kHz.  Values outside this range will just produce the nearest
// allowed frequency.
void OrangutanX2::playFrequency(unsigned int frequency, unsigned int duration)
{
	if (frequency > 0x7FFF)
		frequency = 0x7FFF;		// frequency must be a 15-bit value

	unsigned char *byte_ptr = (unsigned char*)&frequency;
	unsigned char loFreq = *byte_ptr;
	unsigned char hiFreq = *(byte_ptr + 1);

	byte_ptr = (unsigned char*)&duration;
	unsigned char loDur = *byte_ptr;
	unsigned char hiDur = *(byte_ptr + 1);

	// insert the MSBs of the two duration bytes and the low frequency byte
	// MSB of the high frequency byte is guaranteed to be zero
	OrangutanSPIMaster::transmit(160 | ((loDur & 0x80) >> 5) | ((hiDur & 0x80) >> 6) | ((loFreq & 0x80) >> 7));
	OrangutanSPIMaster::transmit(hiFreq);
	OrangutanSPIMaster::transmit(loFreq & 0x7F);
	OrangutanSPIMaster::transmit(hiDur & 0x7F);
	OrangutanSPIMaster::transmit(loDur & 0x7F);
}


// Sets the volume of the buzzer by changing the duty cycle of the pwm driving
// it.  buzzer duty cycle = 1 >> ( 16 - volume ).  volume must range from
// 0 - 15.
void OrangutanX2::setVolume(unsigned char volume)
{
	if (volume > 15)
		volume = 15;		// must be a 4-bit value
	OrangutanSPIMaster::transmit(226);
	OrangutanSPIMaster::transmit(volume);
}


// noteGap determines the silent pause (in ms) that's inserted after every note
// the buzzer plays.  noteGap must be no greater than 127.
void OrangutanX2::setNoteGap(unsigned char noteGap)
{
	if (noteGap > 127)
		noteGap = 127;
	OrangutanSPIMaster::transmit(187);
	OrangutanSPIMaster::transmit(noteGap);
}

#endif
