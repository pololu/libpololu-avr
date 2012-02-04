/*
  OrangutanX2.h - Library of functions for controlling the auxiliary
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

#ifndef OrangutanX2_h
#define OrangutanX2_h

#include "../OrangutanResources/include/OrangutanModel.h"

#ifdef _ORANGUTAN_X2

#include "../OrangutanBuzzer/OrangutanBuzzer.h"		// for note macros


// COMMANDS
#define CMD_MOTOR1_BRAKE_LOW			128
#define CMD_MOTOR1_BRAKE_HIGH			130
#define CMD_MOTOR2_BRAKE_LOW			132
#define CMD_MOTOR2_BRAKE_HIGH			134
#define CMD_MOTOR1_FORWARD				136
#define CMD_MOTOR1_REVERSE				138
#define CMD_MOTOR2_FORWARD				140
#define CMD_MOTOR2_REVERSE				142
#define CMD_JOINT_BRAKE					144	// 150 will also joint-brake low
#define CMD_JOINT_FORWARD				146
#define CMD_JOINT_REVERSE				148
#define CMD_JOINT_ACCEL_FORWARD			228
#define CMD_JOINT_ACCEL_REVERSE			230
#define CMD_MOTOR1_ACCEL_FORWARD		232
#define CMD_MOTOR1_ACCEL_REVERSE		234
#define CMD_MOTOR2_ACCEL_FORWARD		236
#define CMD_MOTOR2_ACCEL_REVERSE		238

#define CMD_DISABLE_JOINT_MOTOR_MODE	214
#define CMD_ENABLE_JOINT_MOTOR_MODE		215
#define CMD_SET_M1_ACCELERATION			208
#define CMD_SET_M2_ACCELERATION			209
#define CMD_SET_M1_BRAKE_DURATION		188
#define CMD_SET_M2_BRAKE_DURATION		190
#define CMD_SET_PWM_FREQUENCIES			210
#define CMD_SET_NUM_CURRENT_SAMPLES		212
#define CMD_SET_M1_CURRENT_LIMIT		192
#define CMD_SET_M2_CURRENT_LIMIT		194

#define CMD_GET_M1_CURRENT				216
#define CMD_GET_M2_CURRENT				217


#define CMD_BUZZER_OFF					225
#define CMD_PLAY_NOTE					152
#define CMD_PLAY_FREQUENCY				160

#define CMD_PLAY_MELODY					176
#define CMD_STORE_NOTE					168
#define CMD_END_MELODY					224
#define CMD_ERASE_MELODIES				186

#define CMD_SET_VOLUME					226
#define CMD_SET_NOTE_GAP				187


#define CMD_SEND_SERIAL					220
#define CMD_READ_SERIAL					219

#define CMD_SET_SERIAL					200
#define CMD_SET_AND_SAVE_SERIAL			201
#define CMD_SET_READ_READY_SIZE			227

#define CMD_GET_SEND_BUFF_FREE_SPACE	222
#define CMD_GET_READ_BUFF_NUM_BYTES		223
#define CMD_GET_UART_ERROR				252


#define CMD_WRITE_EEPROM				240
#define CMD_READ_EEPROM					248
#define CMD_GET_EEPROM_BUSY				254


#define CMD_GET_STATUS					218
#define CMD_GET_FIRMWARE_VERSION		253


// Arguments for the setMotor() method
#define MOTOR1		0
#define MOTOR2		1
#define JOINT_MOTOR	0xFF

#define IMMEDIATE_DRIVE	0
#define ACCEL_DRIVE	1
#define BRAKE_LOW		0xFF

// argument options for setMotorPWM() function
//  PWM frequency = 20MHz / prescaler / 2^(bit-resolution)
//  e.g. PRESCALER_8 with RESOLUTION_7BIT = freq. of 19.5 kHz
#define	RESOLUTION_7BIT				0		// pwm ranges from 0 - 127
#define RESOLUTION_8BIT				1		// pwm ranges from 0 - 255
#define PRESCALER_8					0		// 20MHz / 8
#define PRESCALER_64				1		// 20MHz / 64
#define PRESCALER_256				2		// 20MHz / 256
#define PRESCALER_1024				3		// 20MHz / 1024

// argument options for the setSerial() function
#define UART_READ_BUFF_SZ			32	// UART read buffer can hold 32 bytes
#define UART_SEND_BUFF_SZ			32	// UART send buffer can hold 32 bytes

#define UART_NO_PARITY				0
#define UART_EVEN_PARITY			2
#define UART_ODD_PARITY				3

#define UART_ONE_STOP_BIT			0
#define UART_TWO_STOP_BITS			1

#define UART_NORMAL_SPEED			0
#define UART_DOUBLE_SPEED			1

// UBRR values that achieve various bauds at normal speed
#define UBRR_115200_BAUD			10	// -1.4% error
#define UBRR_76800_BAUD				15	// 1.7% error
#define UBRR_57600_BAUD				21	// -1.4% error
#define UBRR_38400_BAUD				32	// -1.4% error
#define UBRR_28800_BAUD				42	// .9% error
#define UBRR_19200_BAUD				64	// .2% error
#define UBRR_14400_BAUD				86	// -.2% error
#define UBRR_9600_BAUD				129	// .2% error
#define UBRR_4800_BAUD				259	// .2% error
#define UBRR_2400_BAUD				520	// 0% error

// UART error byte bits
#define UART_SEND_BUFF_OVERRUN		1		// UART send buffer overflow
#define UART_READ_BUFF_OVERRUN		2		// UART read buffer overflow
#define UART_FRAME_ERROR			4
#define UART_DATA_OVERRUN			8
#define UART_PARITY_ERROR			16
#define UART_READ_BUFF_UNDERRUN		32		// tried reading from empty buffer

// status byte bits
#define STATUS_UART_ERROR			1	// cleared only on status byte read
#define STATUS_UART_READ_READY		2	// value always reflects current state
#define STATUS_UART_SEND_FULL		4	// value always reflects current state
#define STATUS_BUZZER_FINISHED		8	// value always reflects current state
#define STATUS_M1_FAULT				16	// cleared only on status byte read
#define STATUS_M1_CURRENT_HIGH		32	// cleared only on status byte read
#define STATUS_M2_FAULT				64	// cleared only on status byte read
#define STATUS_M2_CURRENT_HIGH		128	// cleared only on status byte read


// mega168's EEPROM addresses for settings
#define ADDR_INIT_CHECK				0	// check to see if EEPROM initialized
#define ADDR_M1_PWM_FREQUENCY		1	// freq + resolution of PWM1 (timer0)
#define ADDR_M2_PWM_FREQUENCY		2	// freq + resolution of PWM2 (timer2)
#define ADDR_M1_CURRENT_SAMPLES		3	// number of M1 ADC samples to avg
										//   must be a power of 2 <= 128
#define ADDR_M2_CURRENT_SAMPLES		4	// number of M2 ADC samples to avg
										//   must be a power of 2 <= 128
#define ADDR_M1_CURRENT_LIMIT		5	// 0 = no limit
#define ADDR_M1_CL_P_CONST			6	// 7-bit P val: pwm += P * (CL - cur)
										//   0 = shutdown motor on overlimit
#define ADDR_M2_CURRENT_LIMIT		7	// 0 = no limit
#define ADDR_M2_CL_P_CONST			8	// 7-bit P val: pwm += P * (CL - cur)
										//   0 = shutdowm motor on overlimit
#define ADDR_M1_ACCELERATION		9	// M1 acceleration; 0 = instantaneous
#define ADDR_M2_ACCELERATION		10	// M2 acceleration; 0 = instantaneous
#define ADDR_M1_BRAKE_DURATION		11	// M1 time to spend hard braking (ms)
#define ADDR_M2_BRAKE_DURATION		12	// M2 time to spend hard braking (ms)
#define ADDR_SERIAL_SETTINGS		13	// parity, stop bits, 2x speed, p. mode
#define ADDR_SERIAL_UBRRH			14	// UBRRH register (determines baud)
#define ADDR_SERIAL_UBRRL			15	// UBRRL register (determines baud)
#define ADDR_SERIAL_READ_READY		16	// UART read buffer ready for reading
#define ADDR_BUZZER_VOLUME			17	// buzzer volume
#define ADDR_STARTUP_MELODY			18	// melody that plays on startup
										//  0 - 7 = melody, 8 = silence
										//  else chirp
#define ADDR_NOTE_GAP				19	// default duration in ms of silent
										//   pause inserted after each note
#define ADDR_SCK_DURATION			20	// programmer SPI SCK setting
#define ADDR_ISP_STATE				21  // 168 ISP state (in progmode or not)
#define ADDR_ISP_SW_MINOR			22	// ISP version major byte
#define ADDR_ISP_SW_MAJOR			23	// ISP version minor byte


// no pointer to the start of melody0 is needed as location never changes
#define ADDR_MELODY_START_PTR_MSBS	24	// bit i is MSB of melody i+1 pointer
#define ADDR_MELODY1_START_PTR		25	// address of pointer to melody1 start
#define ADDR_MELODY2_START_PTR		26	// melody 2
#define ADDR_MELODY3_START_PTR		27	// melody 3
#define ADDR_MELODY4_START_PTR		28	// melody 4
#define ADDR_MELODY5_START_PTR		29	// melody 5
#define ADDR_MELODY6_START_PTR		30	// melody 6
#define ADDR_MELODY7_START_PTR		31	// melody 7
#define ADDR_MELODY7_END_PTR		32	// ptr to end of melody 7

// address of 1st note of melody 0
#define ADDR_MELODY0_START			33

// there is room in EEPROM for 159 notes, distributed in any way amongst the
// eight melodies.  The mega168's EEPROM is 512 bytes in size.

#ifdef __cplusplus

// C++ Function Declarations

class OrangutanX2
{
  private:
	static void writeToEEPROM(unsigned int address, unsigned char data);
	static unsigned char isEEPROMBusy();

	// Delays execution until the EEPROM on the auxiliary MCU is available for
	// writing or reading.  This is a PRIVATE method.
	static inline void waitForEEPROM()
	{
		while (isEEPROMBusy());
	}


  public:
	static unsigned char getStatus();
	static void getFirmwareVersion(unsigned char &majorVersion, unsigned char &minorVersion);

	// After this function is called, the mega168 must be manually reset for the
	// changes to take effect.  After the reset the settings will all be restored
	//  to their factory default values.
	static inline void restoreDefaultSettings()
	{
		writeToEEPROM(ADDR_INIT_CHECK, 0xFF);
	}
	
	static void saveAVRISPVersion(unsigned char vMajor, unsigned char vMinor);
	
	// This method writes a byte to EEPROM if the specified address is outside
	// the parameter address space.  It can be safely used for general non-volatile
	// data storage using the auxiliary MCU's EEPROM.  If the specified address is
	// inside the parameter address space (i.e. address <= 23), the method
	// does nothing and the data is not stored.
	static inline void saveEEPROMByte(unsigned int address, unsigned char data)
	{
		if (address > 23)
			writeToEEPROM(address, data);
	}

	// This method provides public access to the private writeToEEPROM() method.
	// It is intended to be used for saving X2 parameters, but it can be used to write
	// bytes to any address in the auxiliary MCU's EEPROM (0 - 511).  For saving bytes
	// that are not parameters, the saveEEPROMByte() method should be used so that
	// parameter data cannot be accidentally corrupted.  Parameter addresses are defined
	// in OrangutanX2.h.
	static inline void saveParameter(unsigned int paramAddress, unsigned char paramValue)
	{
		writeToEEPROM(paramAddress, paramValue);
	}
	
	static unsigned char readEEPROMByte(unsigned int address);

	static inline unsigned char readParameter(unsigned int paramAddress)
	{
		return readEEPROMByte(paramAddress);
	}
	
	static void setMotor(unsigned char motor, unsigned char operationMode, int speed);
	static void setPWMFrequencies(unsigned char M1Resolution, unsigned char M1Prescaler,
		unsigned char M2Resolution, unsigned char M2Prescaler, unsigned char save);
	static void setAcceleration(unsigned char motor, unsigned char accel,
		unsigned char save);
	static void setBrakeDuration(unsigned char motor, unsigned char brakeDur,
		unsigned char save);
	static void setNumCurrentSamples( unsigned char M1Exponent, unsigned char M2Exponent,
		unsigned char save);
	static void setCurrentLimit(unsigned char motor, unsigned char limit,
		unsigned char P, unsigned char save);
	static unsigned char getMotorCurrent(unsigned char motor);
	
	static void playNote(unsigned char note, unsigned int duration);
	static void playFrequency(unsigned int frequency, unsigned int duration);
	static void buzzerOff();
	static void setVolume(unsigned char volume, unsigned char save = 0);
	static void setNoteGap(unsigned char noteGap, unsigned char save = 0);
	
	static void setSerial(unsigned char parity, unsigned char stopBits,
		unsigned char speedMode, unsigned int baud_UBRR, unsigned char save);
	static void enablePermanentProgMode(unsigned char save);
	static void setReadReadySize(unsigned char rrSize, unsigned char save);
	static unsigned char getTXBufferSpace();
	static unsigned char getNumRXBytes();
	static unsigned char getSerialError();
	static unsigned char sendSerialByteIfReady(unsigned char data);
	static unsigned char readSerialByte();
};

extern "C" {
#endif // __cplusplus

// C Function Declarations

void x2_get_firmware_version(unsigned char *vmajor, unsigned char *vminor);
unsigned char x2_get_status(void);
void x2_restore_default_settings(void);
void x2_save_avrisp_version(unsigned char vmajor, unsigned char vminor);
void x2_save_eeprom_byte(unsigned int address, unsigned char data);
void x2_save_parameter(unsigned int param_address, unsigned char param_value);
unsigned char x2_read_eeprom_byte(unsigned int address);
unsigned char x2_read_parameter(unsigned int param_address);

void x2_set_motor(unsigned char motor, unsigned char operation_mode, int speed);
void x2_set_pwm_frequencies(unsigned char m1_resolution, unsigned char m1_prescaler,
	unsigned char m2_resolution, unsigned char m2_prescaler, unsigned char save);
void x2_set_acceleration(unsigned char motor, unsigned char accel, unsigned char save);
void x2_set_brake_duration(unsigned char motor, unsigned char brake_dur, unsigned char save);
void x2_set_num_current_samples(unsigned char m1_exp, unsigned char m2_exp, unsigned char save);
void x2_set_current_limit(unsigned char motor, unsigned char limit, unsigned char p,
	unsigned char save);
unsigned char x2_get_motor_current(unsigned char motor);

void x2_play_note(unsigned char note, unsigned int duration);
void x2_play_frequency(unsigned int frequency, unsigned int duration);
void x2_buzzer_off(void);
void x2_set_volume(unsigned char volume, unsigned char save);
void x2_set_note_gap(unsigned char note_gap, unsigned char save);

void x2_set_serial(unsigned char parity, unsigned char stop_bits, unsigned char speed_mode,
	unsigned int baud_ubrr, unsigned char save);
void x2_enable_permanent_programming_mode(unsigned char save);
void x2_set_read_ready_size(unsigned char rrsize, unsigned char save);
unsigned char x2_get_tx_buffer_space(void);
unsigned char x2_get_num_rx_bytes(void);
unsigned char x2_get_serial_error(void);
unsigned char x2_send_serial_byte_if_ready(unsigned char data);
unsigned char x2_read_serial_byte(void);

#ifdef __cplusplus
}
#endif

#endif // _ORANGUTAN_X2

#endif // OrangutanX2_h
