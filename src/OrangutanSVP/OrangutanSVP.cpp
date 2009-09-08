/*
  OrangutanSVP.cpp - Library of functions for the Orangutan SVP 324 and 1284,
      including functions to communicate with auxiliary processor.
*/

/*
 * Written by David Grayson, Sep 2, 2009.
 * Copyright (c) 2009 Pololu Corporation. For more information, see
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

#include "../OrangutanSPIMaster/OrangutanSPIMaster.h"
#include "../OrangutanTime/OrangutanTime.h"
#include "OrangutanSVP.h"

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)

#ifdef LIB_POLOLU

extern "C" unsigned char svp_read_firmware_version()
{
	return OrangutanSVP::readFirmwareVersion();
}

extern "C" unsigned char svp_read_next_byte()
{
	return OrangutanSVP::readNextByte();
}

extern "C" void svp_set_mode(unsigned char mode)
{
	OrangutanSVP::setMode(mode);
}

extern "C" SVPEncoders svp_read_encoders()
{
	return OrangutanSVP::readEncoders();
}

extern "C" unsigned char svp_serial_read_start()
{
	return OrangutanSVP::serialReadStart();
}

extern "C" unsigned char svp_serial_read(char * buffer)
{
	return OrangutanSVP::serialRead(buffer);
}

extern "C" unsigned char svp_serial_send_character_if_ready(char character)
{
	return OrangutanSVP::serialSendIfReady(character);
}

extern "C" void svp_serial_send_character_blocking(char character)
{
	OrangutanSVP::serialSendBlocking(character);
}

extern "C" unsigned char svp_serial_send_if_ready(const char * str)
{
	return OrangutanSVP::serialSendIfReady(str);
}

extern "C" void svp_serial_send_blocking(const char * str)
{
	OrangutanSVP::serialSendBlocking(str);
}

extern "C" unsigned int svp_read_trimpot_millivolts()
{
	return OrangutanSVP::readTrimpotMillivolts();
}

extern "C" SVPStatus svp_read_status()
{
	return OrangutanSVP::readStatus();
}

#endif

typedef union SVPVariables
{
	unsigned char byte[13];
    struct
	{
	    unsigned int channelA;
    	unsigned int channelB;
		unsigned int channelC;
    	unsigned int channelD;
    	unsigned int trimpot;
    	unsigned int battery;
		SVPStatus status;  // see OrangutanSVP.h or svp.h
	};
} SVPVariables;

/* GLOBAL VARIABLES ***********************************************************/

/* A cache of all the variables from the SVP's auxilliary processor. **********/
// Battery is initialized the invalid value 0xFFFF so that our library can tell
// when the variables have never been updated and be sure to update them when
// updateVariablesIfNeeded is first called.
static SVPVariables svp_variables = {battery:0xFFFF};

/* LOW-LEVEL FUNCTIONS FOR DOING SPI COMMUNICATION ****************************/
// All the delays in these functions were chosen by doing an analysis of the
// auxiliary processor's assembly code for handling SPI communication.

unsigned char OrangutanSVP::readFirmwareVersion()
{
	OrangutanSPIMaster::transmitAndDelay(0x80, 5);
	return readNextByte();
}

unsigned char OrangutanSVP::readNextByte()
{
	return OrangutanSPIMaster::transmitAndDelay(0xFF, 4);
}

static void updateVariables()
{
    OrangutanSPIMaster::transmitAndDelay(0x81, 7);

	for(unsigned char i=0; i < sizeof(SVPVariables); i++)
	{
		svp_variables.byte[i] = OrangutanSVP::readNextByte();
	}
}

SVPEncoders OrangutanSVP::readEncoders()
{
	SVPEncoders encoders;
	OrangutanSPIMaster::transmitAndDelay(0x82, 6);

	for(unsigned char i=0; i < sizeof(SVPEncoders); i++)
	{
		encoders.byte[i] = OrangutanSVP::readNextByte();
	}

	return encoders;
}

// Issues the Read Port Bytes command and returns the number of bytes there are to read.
// Those bytes should then be read with calls to readNextByte().
unsigned char OrangutanSVP::serialReadStart()
{
    OrangutanSPIMaster::transmitAndDelay(0x83, 7);
	return readNextByte();
}

unsigned char OrangutanSVP::serialRead(char * buffer)
{
	unsigned char byteCount = serialReadStart();
	for (unsigned char i=0; i < byteCount; i++)
	{
		buffer[i] = readNextByte();
	}
	return byteCount;
}

unsigned char OrangutanSVP::serialSendIfReady(char byte)
{
    OrangutanSPIMaster::transmitAndDelay(byte & 0x80 ? 0x85 : 0x84, 5);
	return OrangutanSPIMaster::transmitAndDelay(byte & 0x7F, 5);
}

unsigned char OrangutanSVP::serialSendIfReady(const char * str)
{
	while (*str != 0)
	{
		if (0xFF != serialSendIfReady(*str))
		{
			return 0;
		}
		str++;
	}
	return 0xFF;
}

void OrangutanSVP::serialSendBlocking(char byte)
{
	while(0xFF != serialSendIfReady(byte)){}
}

void OrangutanSVP::serialSendBlocking(const char * str)
{
	while (*str != 0)
	{
		serialSendBlocking(*str);
		str++;
	}
}

/* setMode: Sets the current mode of the SVP's auxiliary processor.
   The mode parameter should be an inclusive or of
     1) MODE_RX, MODE_ANALOG, or MODE_ENCODERS
     2) MODE_SLAVE_SELECT or 0
 */
void OrangutanSVP::setMode(unsigned char mode)
{
	// When the auxiliary processor starts up, it is in SVP_MODE_RX.
	static unsigned char svp_mode = SVP_MODE_RX;

	if (svp_mode == mode)
	{
		// The auxilliary processor is already in the correct mode,
		// so don't actually set it.
		return;
	}

	svp_mode = mode;
	OrangutanSPIMaster::transmitAndDelay(0xC0 | svp_mode, 5);
}

/* HIGH-LEVEL FUCNTIONS FOR HANDLING CACHING **********************************/

static void updateVariablesIfNeeded()
{
    // The value of ms() from the last time the svp_variables was updated.
	static unsigned long svp_variables_last_update_ms;
	
	if (svp_variables.battery == 0xFFFF || OrangutanTime::ms() - svp_variables_last_update_ms >= 2)
	{
		svp_variables_last_update_ms = OrangutanTime::ms();
		updateVariables();
	}
}

unsigned int OrangutanSVP::readTrimpotMillivolts()
{
	// TODO: use setMode to disable slave select if it is enabled?  Otherwise,
	// if slave select is enabled, then the reading will just be 0xFFFF.

	updateVariablesIfNeeded();
	return svp_variables.trimpot;
}

unsigned int OrangutanSVP::readBatteryMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.battery;
}

unsigned int OrangutanSVP::readChannelAMillivolts()
{
	// TODO: if necessary, set the mode and delay?
	updateVariablesIfNeeded();
	return svp_variables.channelA;
}

unsigned int OrangutanSVP::readChannelBMillivolts()
{
	// TODO: if necessary, set the mode and delay?
	updateVariablesIfNeeded();
	return svp_variables.channelB;
}

unsigned int OrangutanSVP::readChannelCMillivolts()
{
	// TODO: if necessary, set the mode and delay?
	updateVariablesIfNeeded();
	return svp_variables.channelC;
}

unsigned int OrangutanSVP::readChannelDMillivolts()
{
	// TODO: if necessary, set the mode and delay?
	updateVariablesIfNeeded();
	return svp_variables.channelD;
}

SVPStatus OrangutanSVP::readStatus()
{
	updateVariablesIfNeeded();
	return svp_variables.status;
}

#endif
