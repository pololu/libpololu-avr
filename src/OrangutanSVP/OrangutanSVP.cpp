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

extern "C" SVPVariables svp_read_variables()
{
	return OrangutanSVP::readVariables();
}

extern "C" unsigned char svp_serial_read_start()
{
	return OrangutanSVP::serialReadStart();
}

extern "C" unsigned char svp_serial_read(char * buffer)
{
	return OrangutanSVP::serialRead(buffer);
}

extern "C" unsigned char svp_serial_send_if_ready(char data)
{
	return OrangutanSVP::serialSendIfReady(data);
}

extern "C" void svp_serial_send_blocking(char data)
{
	OrangutanSVP::serialSendBlocking(data);
}

#endif

unsigned char OrangutanSVP::readFirmwareVersion()
{
	OrangutanSPIMaster::transmitAndDelay(0x80, 4);
	return readNextByte();
}

unsigned char OrangutanSVP::readNextByte()
{
	return OrangutanSPIMaster::transmitAndDelay(0xFF, 3);
}

void OrangutanSVP::setMode(unsigned char mode)
{
	OrangutanSPIMaster::transmitAndDelay(0xC0 | mode, 4);
}

SVPVariables OrangutanSVP::readVariables()
{
	SVPVariables vars;
    OrangutanSPIMaster::transmitAndDelay(0x81, 6);

	for(unsigned char i=0; i < sizeof(SVPVariables); i++)
	{
		vars.byte[i] = readNextByte();
	}

	return vars;
}

SVPEncoders OrangutanSVP::readEncoders()
{
	SVPEncoders encoders;
	OrangutanSPIMaster::transmitAndDelay(0x82, 5);

	for(unsigned char i=0; i < sizeof(SVPEncoders); i++)
	{
		encoders.byte[i] = readNextByte();
	}

	return encoders;
}

// Issues the Read Port Bytes command and returns the number of bytes there are to read.
// Those bytes should then be read with calls to readNextByte().
unsigned char OrangutanSVP::serialReadStart()
{
    OrangutanSPIMaster::transmitAndDelay(0x83, 6);
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
    OrangutanSPIMaster::transmitAndDelay(byte & 0x80 ? 0x85 : 0x84, 4);
	return OrangutanSPIMaster::transmitAndDelay(byte & 0x7F, 4);
}

void OrangutanSVP::serialSendBlocking(char byte)
{
	while(0xFF != serialSendIfReady(byte)){}
}

#endif
