/*
  OrangutanX2.cpp - Library of functions for controlling the auxiliary
	processor on the Orangutan X2.
*/

/*
 * Written by Ben Schmidel, Mar 2, 2010.
 * Copyright (c) 2010-2012 Pololu Corporation. For more information, see
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


extern "C" void x2_get_firmware_version(unsigned char *vmajor, unsigned char *vminor)
{
	OrangutanX2::getFirmwareVersion(*vmajor, *vminor);
}

extern "C" unsigned char x2_get_status()
{
	return OrangutanX2::getStatus();
}

extern "C" void x2_restore_default_settings()
{
	OrangutanX2::restoreDefaultSettings();
}

extern "C" void x2_save_avrisp_version(unsigned char vmajor, unsigned char vminor)
{
	OrangutanX2::saveAVRISPVersion(vmajor, vminor);
}

extern "C" void x2_save_eeprom_byte(unsigned int address, unsigned char data)
{
	OrangutanX2::saveEEPROMByte(address, data);
}

extern "C" void x2_save_parameter(unsigned int param_address, unsigned char param_value)
{
	OrangutanX2::saveParameter(param_address, param_value);
}

extern "C" unsigned char x2_read_eeprom_byte(unsigned int address)
{
	return OrangutanX2::readEEPROMByte(address);
}

extern "C" unsigned char x2_read_parameter(unsigned int param_address)
{
	return OrangutanX2::readParameter(param_address);
}

// ***************** MOTORS *****************

extern "C" void x2_set_motor(unsigned char motor, unsigned char operation_mode, int speed)
{
	OrangutanX2::setMotor(motor, operation_mode, speed);
}

extern "C" void x2_set_pwm_frequencies(unsigned char m1_resolution, unsigned char m1_prescaler,
	unsigned char m2_resolution, unsigned char m2_prescaler, unsigned char save)
{
	OrangutanX2::setPWMFrequencies(m1_resolution, m1_prescaler, m2_resolution, m2_prescaler, save);
}

extern "C" void x2_set_acceleration(unsigned char motor, unsigned char accel, unsigned char save)
{
	OrangutanX2::setAcceleration(motor, accel, save);
}

extern "C" void x2_set_brake_duration(unsigned char motor, unsigned char brake_dur, unsigned char save)
{
	OrangutanX2::setBrakeDuration(motor, brake_dur, save);
}

extern "C" void x2_set_num_current_samples(unsigned char m1_exp, unsigned char m2_exp, unsigned char save)
{
	OrangutanX2::setNumCurrentSamples(m1_exp, m2_exp, save);
}

extern "C" void x2_set_current_limit(unsigned char motor, unsigned char limit, unsigned char p,
	unsigned char save)
{
		OrangutanX2::setCurrentLimit(motor, limit, p, save);
}

extern "C" unsigned char x2_get_motor_current(unsigned char motor)
{
	return OrangutanX2::getMotorCurrent(motor);
}

// ***************** BUZZER *****************

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

extern "C" void x2_set_volume(unsigned char volume, unsigned char save)
{
	OrangutanX2::setVolume(volume, save);
}

extern "C" void x2_set_note_gap(unsigned char note_gap, unsigned char save)
{
	OrangutanX2::setNoteGap(note_gap, save);
}

// ***************** UART (SERIAL) *****************

extern "C" void x2_set_serial(unsigned char parity, unsigned char stop_bits, unsigned char speed_mode,
	unsigned int baud_ubrr, unsigned char save)
{
	OrangutanX2::setSerial(parity, stop_bits, speed_mode, baud_ubrr, save);
}

extern "C" void x2_enable_permanent_programming_mode(unsigned char save)
{
	OrangutanX2::enablePermanentProgMode(save);
}

extern "C" void x2_set_read_ready_size(unsigned char rrsize, unsigned char save)
{
	OrangutanX2::setReadReadySize(rrsize, save);
}

extern "C" unsigned char x2_get_tx_buffer_space()
{
	return OrangutanX2::getTXBufferSpace();
}

extern "C" unsigned char x2_get_num_rx_bytes()
{
	return OrangutanX2::getNumRXBytes();
}

extern "C" unsigned char x2_get_serial_error()
{
	return OrangutanX2::getSerialError();
}

extern "C" unsigned char x2_send_serial_byte_if_ready(unsigned char data)
{
	return OrangutanX2::sendSerialByteIfReady(data);
}

extern "C" unsigned char x2_read_serial_byte()
{
	return OrangutanX2::readSerialByte();
}


// After this method is called, the two arguments will hold the values of the major firmware
// version byte and minor firmware version byte.  The firmware version is
// majorVersion.minorVersion
void OrangutanX2::getFirmwareVersion(unsigned char &majorVersion, unsigned char &minorVersion)
{
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_FIRMWARE_VERSION, 3);
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
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_STATUS, 3);
	return OrangutanSPIMaster::transmit(0);
}


// At the time of the X2's release, the current AVR Studio AVRISP version is
// 2.0A. The arguments to this function represent the version AVRISP version
// number the mega168 will transmit to AVR Studio when you attempt to program
// the mega644. If the two version don't match, you will have to first cancel
// out of an "upgrade your firmware?" dialog box every time you try to program
// your mega644 (you cannot upgrade your firmware, anyway, so this dialog is
// nothing more than an annoyance). As such, you might have a better
// programming experience if you set these version values to match the AVRISP
// version used by the AVR Studio you're running (if you use AVR Studio).
void OrangutanX2::saveAVRISPVersion(unsigned char vMajor, unsigned char vMinor)
{
	// version is "vMajor.vMinor"
	writeToEEPROM(ADDR_ISP_SW_MAJOR, vMajor);
	writeToEEPROM(ADDR_ISP_SW_MINOR, vMinor);
}


// Write a byte to the specified mega168's EEPROM address.  This command
// can be used to store settings so that they persist after a hardware reset.
// Settings occupy EEPROM bytes 0 - 23 and melody pointers occupy bytes
// 24 - 32.  Melody notes are saved to EEPROM in three-byte blocks starting
// at address 33.
// This method is PRIVATE and should be accessed using the public methods
// saveEEPROMByte() and saveParameter()
void OrangutanX2::writeToEEPROM(unsigned int address, unsigned char data)
{
	if (address >= 512)		// address out of bounds
		return;

	waitForEEPROM();		// wait for any current EEPROM writes to finish

	// insert data MSB and address bits 7 and 8 into the command byte
	OrangutanSPIMaster::transmit(CMD_WRITE_EEPROM | ((data & 0x80) >> 5)
								  | ((address & 0x0080) >> 6)
								  | ((address & 0x0100) >> 8));
	OrangutanSPIMaster::transmit((unsigned char)(address & 0x007F));
	OrangutanSPIMaster::transmit(data & 0x7F);
}


// Read a byte from the mega168's EEPROM.  This command can be used to check
// the values of the settings that are loaded when the 168 is reset.
// Settings occupy EEPROM bytes 0 - 23.
unsigned char OrangutanX2::readEEPROMByte(unsigned int address)
{
	if (address >= 512)		// address out of bounds
		return 0;

	waitForEEPROM();		// wait for any current EEPROM writes to finish
	
	// insert address bits 7 and 8 into the command byte
	OrangutanSPIMaster::transmit(CMD_READ_EEPROM | ((address & 0x0080) >> 6)
								 | ((address & 0x0100) >> 8));
	OrangutanSPIMaster::transmitAndDelay((unsigned char)(address & 0x007F), 3);
	return OrangutanSPIMaster::transmit(0);		// send a junk data byte here
}


// Check to see if the mega168's EEPROM is currently being written to, which
// means it is not possible to read from it or start a new write.  This is
// a PRIVATE method.
unsigned char OrangutanX2::isEEPROMBusy()
{
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_EEPROM_BUSY, 3);
	return OrangutanSPIMaster::transmit(0);		// send a junk data byte here
}



//****************************************************************************
// Motor
//****************************************************************************


// motor: 0 for M1, 1 for M2, 0xFF for joint
// operationMode: 0 for immediate drive, 1 for acceleration drive, 0xFF for brake low
// speed: -255 to 255 (absolute value of speed used when braking, otherwise sign = direction)
void OrangutanX2::setMotor(unsigned char motor, unsigned char operationMode, int speed)
{
	static unsigned char inJointMotorMode = 0;

	if (motor == JOINT_MOTOR && !inJointMotorMode)
	{
		OrangutanSPIMaster::transmit(CMD_ENABLE_JOINT_MOTOR_MODE);	// enable joint-motor mode
		inJointMotorMode = 1;
	}
	if (motor != JOINT_MOTOR && inJointMotorMode)
	{
		OrangutanSPIMaster::transmit(CMD_DISABLE_JOINT_MOTOR_MODE);	// disable joint-motor mode
		inJointMotorMode = 0;
	}
	unsigned char cmd;

	if (motor == JOINT_MOTOR)					// if joint motor command
	{
		if (operationMode == ACCEL_DRIVE)		// if accel
			cmd = CMD_JOINT_ACCEL_FORWARD;			// joint motor accel forward
		else if (operationMode == BRAKE_LOW)	// if brake
			cmd = CMD_JOINT_BRAKE;					// joint motor brake low
		else									// else immediate drive
			cmd = CMD_JOINT_FORWARD;				// joint motor forward
	}
	else
	{
		if (motor != MOTOR2)
			motor = MOTOR1;
		if (operationMode == ACCEL_DRIVE)		// if accel
			cmd = CMD_MOTOR1_ACCEL_FORWARD + motor * 4;		// motor accel forward
		else if (operationMode == BRAKE_LOW)	// if brake
			cmd = CMD_MOTOR1_BRAKE_LOW + motor * 4;		// motor brake low
		else									// else immediate drive
			cmd = CMD_MOTOR1_FORWARD + motor * 4;		// motor forward
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


// PWM frequency is determined by the resolution (either 7-bit or 8-bit) and
// prescaler used to convert the 20MHz IO clk into the PWM clock.  The formula
// is frequency = 20MHz / prescaler / 2^(bit-resolution).
// For example: 7-bit resolution with a prescaler of 8 gives PWM freq = 19.5 kHz.
// Note that VHN2SP30 motor drivers can handle PWMs up to 20 kHz, but VNH3SP30
// motor drivers can only handle PWMs up to 10 kHz, so you should not use
// 7-bit resolution with a prescaler of 8 if your Orangutan X2 has VNH3SP30
// motor drivers.
// Arguments passed to this function should be the RESOLUTION_ and PRESCALER_
// #defines in OrangutanX2.h.
// Default resolution is 8-bit and default presaler is 8, so default PWM frequency
// is 9.8 kHz.
void OrangutanX2::setPWMFrequencies(unsigned char M1Resolution, unsigned char M1Prescaler,
	unsigned char M2Resolution, unsigned char M2Prescaler, unsigned char save)
{
	if ( M1Resolution != RESOLUTION_7BIT )
		M1Resolution = RESOLUTION_8BIT;
	if ( M2Resolution != RESOLUTION_7BIT )
		M2Resolution = RESOLUTION_8BIT;
	if ( M1Prescaler > PRESCALER_1024 )
		M1Prescaler = PRESCALER_1024;
	if ( M2Prescaler > PRESCALER_1024 )
		M2Prescaler = PRESCALER_1024;

	OrangutanSPIMaster::transmit(CMD_SET_PWM_FREQUENCIES);
	OrangutanSPIMaster::transmit((M2Resolution << 5) | (M2Prescaler << 3) |
		(M1Resolution << 2) | M1Prescaler);
	if (save)
	{
		writeToEEPROM(ADDR_M1_PWM_FREQUENCY, (M1Resolution << 2) | M1Prescaler);
		writeToEEPROM(ADDR_M2_PWM_FREQUENCY, (M2Resolution << 2) | M2Prescaler);
	}
}


// Sets the accelerations used by the accelMotor functions. Acceleration commands
// cause the motor's pwm to be incremented by accel/10 every 10ms.  Acceleration
// only applies when the PWM duty cycle is increasing or when the motor changes
// direction.  Decreases in PWM duty cycle occur immediately (i.e. there is no
// deceleration).  The net effect is that the pwm increases by accel every 100ms
// when the motor is commanded to increase speed.  When the motor is commanded
// to change direction, the PWM duty cycle is immediately set to zero, a brake
// duration occurs if the brake duration parameter is nonzero, and then the
// PWM duty cycle increases from zero to the target speed at a rate of accel
// per 100 ms.
// An accel of zero = "infinite" acceleration.  Accel must be 127 or less.
// Default acceleration is 25 (i.e. 1 second to go from speed 0 to 250).
void OrangutanX2::setAcceleration(unsigned char motor, unsigned char accel,
	unsigned char save)
{
	if (accel > 127)
		accel= 127;
	unsigned char cmd = CMD_SET_M1_ACCELERATION;
	unsigned char param = ADDR_M1_ACCELERATION;
	if (motor == MOTOR2)
	{
		cmd = CMD_SET_M2_ACCELERATION;
		param = ADDR_M2_ACCELERATION;
	}
	OrangutanSPIMaster::transmit(cmd);
	OrangutanSPIMaster::transmit(accel);
	if (save)
		writeToEEPROM(param, accel);
}


// Sets the amount of time the motor will brake low at 100% duty cycle when
// it receives an acceleration command that will result in a change of
// direction.  The units of brakeDuration are 10ms, which means an argument
// value of 1 will result in a brake duration of 10ms and an argument value
// of 127 will result in a brake duration of 1.27s.  If brakeDuration is zero,
// there is no braking before an acceleration direction change.  brakeDuration
// must be 127 or less.  These functions will only work with X2 firmware
// versions 1.01 or later.
// Default brake duration is 10 (i.e. brake for 100ms on direction change).
void OrangutanX2::setBrakeDuration(unsigned char motor, unsigned char brakeDur,
	unsigned char save)
{
	if (brakeDur > 127)
		brakeDur = 127;
	unsigned char cmd = CMD_SET_M1_BRAKE_DURATION;
	unsigned char param = ADDR_M1_BRAKE_DURATION;
	if (motor == MOTOR2)
	{
		cmd = CMD_SET_M2_BRAKE_DURATION;
		param = ADDR_M2_BRAKE_DURATION;
	}
	OrangutanSPIMaster::transmit(cmd);
	OrangutanSPIMaster::transmit(brakeDur);
	if (save)
		writeToEEPROM(param, brakeDur);
}


// The number of current samples in the running current averages is given by:
// M1 average current averages the last 2 ^ M1Exponent samples
// M2 average current averages the last 2 ^ M2Exponent samples
// The exponents may range from 0 - 7 (which equates to 1, 2, 4, 8, 16, 32,
// 64, or 128 samples).  More samples means less noise, but it also means
// older data is affecting your present measurement.
// Note: measuring current is only possible on Orangutan X2s with VNH2SP30
// motor drivers.  The VNH3SP30 does not support current sense feedback.
// Default number of averaged current samples is 128 (i.e. default exponent is 7).
void OrangutanX2::setNumCurrentSamples( unsigned char M1Exponent, unsigned char M2Exponent,
	unsigned char save)
{
	if ( M1Exponent > 7 )
		M1Exponent = 7;
	if ( M2Exponent > 7 )
		M2Exponent = 7;
	OrangutanSPIMaster::transmit(CMD_SET_NUM_CURRENT_SAMPLES);
	OrangutanSPIMaster::transmit((M2Exponent << 3) | M1Exponent);
	if (save)
	{
		writeToEEPROM(ADDR_M1_CURRENT_SAMPLES, 1 << M1Exponent);
		writeToEEPROM(ADDR_M2_CURRENT_SAMPLES, 1 << M2Exponent);
	}
}


// If current limit is zero, there is no limit.  Otherwise, if the current
// average ever exceeds the limit, P determines what happens.  If P is zero,
// the motor shuts off.  If P is non-zero, the motor's PWM is decreased by
// P * (current - limit).  The PWM will never increase by an amount greater
// than P * | limit - current | while accelerating, either.  There are
// no restrictions on limit; P must be no greater than 127.  These functions
// will only work with X2 firmware version 1.01 or later.
// Note: current limiting is only possible on Orangutan X2s with VNH2SP30
// motor drivers.  The VNH3SP30 does not support current sense feedback.
// Default current limit is 0 (i.e. no current limit) and default
// proportionality constant P is 5.
void OrangutanX2::setCurrentLimit(unsigned char motor, unsigned char limit,
	unsigned char P, unsigned char save)
{
	if (P > 127)
		P = 127;
	unsigned char cmd = CMD_SET_M1_CURRENT_LIMIT;
	unsigned char param1 = ADDR_M1_CURRENT_LIMIT;
	unsigned char param2 = ADDR_M1_CL_P_CONST;
	if (motor == MOTOR2)
	{
		cmd = CMD_SET_M2_CURRENT_LIMIT;
		param1 = ADDR_M2_CURRENT_LIMIT;
		param2 = ADDR_M2_CL_P_CONST;
	}
	OrangutanSPIMaster::transmit(limit & 0x80 ? cmd | 1 : cmd);
	OrangutanSPIMaster::transmit(limit & 0x7F);
	OrangutanSPIMaster::transmit(P);
	if (save)
	{
		writeToEEPROM(param1, limit);
		writeToEEPROM(param2, P);
	}
}


// Get the running current average for the specified motor
// Note: measuring current is only possible on Orangutan X2s with VNH2SP30
// motor drivers.  The VNH3SP30 does not support current sense feedback.
unsigned char OrangutanX2::getMotorCurrent(unsigned char motor)
{
	unsigned char cmd = CMD_GET_M1_CURRENT;
	if (motor == MOTOR2)
		cmd = CMD_GET_M2_CURRENT;
	OrangutanSPIMaster::transmitAndDelay(cmd, 3);
	return OrangutanSPIMaster::transmit(0);
}



//****************************************************************************
// Buzzer
//****************************************************************************


// Silence the buzzer immediately
void OrangutanX2::buzzerOff()
{
	OrangutanSPIMaster::transmit(CMD_BUZZER_OFF);
}


// Play the specified note for the specified duration (in ms) immediately.
// Note enumeration is provided as a set of #define macros in OrangutanBuzzer.h.
void OrangutanX2::playNote(unsigned char note, unsigned int duration)
{
	unsigned char *byte_ptr = (unsigned char*)&duration;
	unsigned char lo = *byte_ptr;		// low byte of duration
	unsigned char hi = *(byte_ptr + 1);	// high byte of duration
	unsigned char cmd = CMD_PLAY_NOTE;
	if (lo & 0x80)
		cmd |= 4;
	if (hi & 0x80)
		cmd |= 2;
	if (note & 0x80)
		cmd |= 1;
	// insert the MSBs of the three data bytes into the command byte
	OrangutanSPIMaster::transmit(cmd);
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
	
	unsigned char cmd = CMD_PLAY_FREQUENCY;
	if (loDur & 0x80)
		cmd |= 4;
	if (hiDur & 0x80)
		cmd |= 2;
	if (loFreq & 0x80)
		cmd |= 1;

	// insert the MSBs of the two duration bytes and the low frequency byte
	// MSB of the high frequency byte is guaranteed to be zero
	OrangutanSPIMaster::transmit(cmd);
	OrangutanSPIMaster::transmit(hiFreq);
	OrangutanSPIMaster::transmit(loFreq & 0x7F);
	OrangutanSPIMaster::transmit(hiDur & 0x7F);
	OrangutanSPIMaster::transmit(loDur & 0x7F);
}


// Sets the volume of the buzzer by changing the duty cycle of the pwm driving
// it.  buzzer duty cycle = 1 >> ( 16 - volume ).  volume must range from
// 0 - 15.  Default volume is 12.
void OrangutanX2::setVolume(unsigned char volume, unsigned char save)
{
	if (volume > 15)
		volume = 15;		// must be a 4-bit value
	OrangutanSPIMaster::transmit(CMD_SET_VOLUME);
	OrangutanSPIMaster::transmit(volume);
	if (save)
		writeToEEPROM(ADDR_BUZZER_VOLUME, volume);
}


// noteGap determines the silent pause (in ms) that's inserted after every note
// the buzzer plays.  noteGap must be no greater than 127.
// Default note gap is 5 (5ms pause between notes).
void OrangutanX2::setNoteGap(unsigned char noteGap, unsigned char save)
{
	if (noteGap > 127)
		noteGap = 127;
	OrangutanSPIMaster::transmit(CMD_SET_NOTE_GAP);
	OrangutanSPIMaster::transmit(noteGap);
	if (save)
		writeToEEPROM(ADDR_NOTE_GAP, noteGap);
}



//****************************************************************************
// UART
//****************************************************************************


// Sets the UART parameters.  Arguments to this function should be the
// UART #defines in OrangutahX2.h.  speedMode is either UART_NORMAL_SPEED or
// UART_DOUBLE_SPEED.  This command will disable permanent progmode
// if it's enabled.  If the argument "save" is nonzero, the serial parameters
// are also saved to EEPROM and will persist after a power cycle.
// Default settings are: normal UART functionality, no parity, one stop bit,
// normal speed (i.e. not double-speed mode), 115.2kbps baud rate.  The
// UART character size is always 8-bits.
void OrangutanX2::setSerial(unsigned char parity, unsigned char stopBits,
	unsigned char speedMode, unsigned int baud_UBRR, unsigned char save)
{
	// ensure parameter values are all valid
	if ((parity != UART_EVEN_PARITY) && (parity != UART_ODD_PARITY))
		parity = UART_NO_PARITY;
	if (stopBits != UART_ONE_STOP_BIT)
		stopBits = UART_TWO_STOP_BITS;
	if (speedMode != UART_NORMAL_SPEED)
		speedMode = UART_DOUBLE_SPEED;
	if (baud_UBRR > 2047)					// UBRR must be an 11-bit value
		baud_UBRR = 2047;
	unsigned char cmd = CMD_SET_SERIAL;
	if (save)
	{
		cmd = CMD_SET_AND_SAVE_SERIAL;
		waitForEEPROM();
	}
	OrangutanSPIMaster::transmit(cmd | (parity << 1));
	OrangutanSPIMaster::transmit((stopBits << 5) | (speedMode << 4) |
				((baud_UBRR & 0x0780) >> 7));	// send the four MSBs of UBRR
	OrangutanSPIMaster::transmit(baud_UBRR & 0x007F);	// send the seven LSBs of UBBR
}


// This command reserves the UART solely for programming the mega644.  In
// permanent program mode you do not have to explicitly put the X2 into
// programming mode to program the mega644.  This command overrides all
// current UART settings.  This setting can be made to persist after a 
// hardware reset by saving the value 0x40 to mega168 EEPROM address 13.
void OrangutanX2::enablePermanentProgMode(unsigned char save)
{
	OrangutanSPIMaster::transmit(CMD_SET_SERIAL);
	OrangutanSPIMaster::transmit(0x40);
	OrangutanSPIMaster::transmit(0);
	if (save)
		writeToEEPROM(ADDR_SERIAL_SETTINGS, 0x40);
}


// Determines how many bytes must be in the UART read buffer before the status
// byte and attention line indicate that it is ready to be read.
// Default value is 1 (i.e. read-ready flag set after every received byte).
void OrangutanX2::setReadReadySize(unsigned char rrSize, unsigned char save)
{
	// rrSize must be in the range of 1 - UART_READ_BUFF_SZ (32)
	if (rrSize == 0)
		rrSize = 1;
	if (rrSize > UART_READ_BUFF_SZ)
		rrSize = UART_READ_BUFF_SZ;
	OrangutanSPIMaster::transmit(CMD_SET_READ_READY_SIZE);
	OrangutanSPIMaster::transmit(rrSize - 1);	// transmit one less than desired value
	if (save)
		writeToEEPROM(ADDR_SERIAL_READ_READY, rrSize);
}


// Get number of unused bytes in the UART send buffer
unsigned char OrangutanX2::getTXBufferSpace()
{
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_SEND_BUFF_FREE_SPACE, 3);
	return OrangutanSPIMaster::transmit(0);			// junk data byte
}


// Get number of unread bytes in the UART read buffer
unsigned char OrangutanX2::getNumRXBytes()
{
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_READ_BUFF_NUM_BYTES, 3);
	return OrangutanSPIMaster::transmit(0);			// junk data byte
}


// When a serial error occurs, the appropriate bit in the UART error byte is set and
// latched, and the STATUS_UART_ERROR bit of the status byte is set and the X2's
// ATT line is set.  Calling this function returns the UART error byte and then
// clears it.  The error byte bits are enumerated in OrangutanX2.h.
unsigned char OrangutanX2::getSerialError()
{
	OrangutanSPIMaster::transmitAndDelay(CMD_GET_UART_ERROR, 3);
	return OrangutanSPIMaster::transmit(0);
}


// Send a byte to the mega168 to be queued for transmission over the UART.
// While queued the byte will reside in the mega168's UART send buffer.
// If there is no room in the transmit buffer for the byte to be sent, this method
// returns 0, otherwise it returns 1.
unsigned char OrangutanX2::sendSerialByteIfReady(unsigned char data)
{
	if (getTXBufferSpace() == 0)
		return 0;
	OrangutanSPIMaster::transmit(data & 0x80 ? CMD_SEND_SERIAL | 1 : CMD_SEND_SERIAL);
	OrangutanSPIMaster::transmit(data & 0x7F);
	return 1;
}


// Retrieve the next byte from the mega168's UART read buffer.
unsigned char OrangutanX2::readSerialByte()
{
	OrangutanSPIMaster::transmitAndDelay(CMD_READ_SERIAL, 3);
	return OrangutanSPIMaster::transmit(0);		// junk data byte
}
#endif
