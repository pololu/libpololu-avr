/*
  OrangutanSVP.cpp - Library of functions for the Orangutan SVP 324 and 1284,
      including functions to communicate with auxiliary processor.
*/

/*
 * Written by David Grayson, Sep 2, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
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
#include "OrangutanSVP.h"

#ifdef _ORANGUTAN_SVP


extern "C" unsigned char svp_get_firmware_version()
{
	return OrangutanSVP::getFirmwareVersion();
}

extern "C" void svp_set_mode(unsigned char mode)
{
	OrangutanSVP::setMode(mode);
}

extern "C" SVPStatus svp_get_status()
{
	return OrangutanSVP::getStatus();
}

extern "C" int svp_get_counts_ab()
{
	return OrangutanSVP::getCountsAB();
}

extern "C" int svp_get_counts_and_reset_ab()
{
	return OrangutanSVP::getCountsAndResetAB();
}

extern "C" int svp_get_counts_cd()
{
	return OrangutanSVP::getCountsCD();
}

extern "C" int svp_get_counts_and_reset_cd()
{
	return OrangutanSVP::getCountsAndResetCD();
}

extern "C" unsigned char svp_check_error_ab()
{
	return OrangutanSVP::checkErrorAB();
}

extern "C" unsigned char svp_check_error_cd()
{
	return OrangutanSVP::checkErrorCD();
}


typedef union SVPVariables
{
	unsigned char byte[13];
    struct
	{
		SVPStatus status;  // see OrangutanSVP.h
	    unsigned int channelA;
    	unsigned int channelB;
		unsigned int channelC;
    	unsigned int channelD;
    	unsigned int trimpot;
    	unsigned int battery;
	};
} SVPVariables;

typedef union SVPEncoders
{
	unsigned char byte[5];
	struct
	{
		unsigned int countAB;
		unsigned int countCD;
		union
		{
			struct
			{
				unsigned errorAB :1;
				unsigned errorCD :1;
			};
			unsigned char status;
		};
	};
} SVPEncoders;

int lastCountAB;
int lastCountCD;

/* GLOBAL VARIABLES ***********************************************************/

/* A cache of all the variables from the SVP's auxilliary processor. **********/
// Battery is initialized the invalid value 0xFFFF so that our library can tell
// when the variables have never been updated and be sure to update them when
// updateVariablesIfNeeded is first called.
static SVPVariables svp_variables;

static SVPEncoders encoders;

/* LOW-LEVEL FUNCTIONS FOR DOING SPI COMMUNICATION ****************************/
// All the delays in these functions were chosen by doing an analysis of the
// auxiliary processor's assembly code for handling SPI communication.

unsigned char OrangutanSVP::getFirmwareVersion()
{
	OrangutanSPIMaster::transmitAndDelay(0x80, 5);
	return getNextByte();
}

unsigned char OrangutanSVP::getNextByte()
{
	return OrangutanSPIMaster::transmitAndDelay(0xFF, 4);
}

static void updateVariables()
{
    OrangutanSPIMaster::transmitAndDelay(0x81, 7);

	for(unsigned char i=0; i < sizeof(SVPVariables); i++)
	{
		svp_variables.byte[i] = OrangutanSVP::getNextByte();
	}
}

SVPEncoders updateEncoders()
{
	OrangutanSPIMaster::transmitAndDelay(0x82, 6);
	
	// Read the total number of counts seen on AB (encoders.countAB).
	encoders.byte[0] = OrangutanSVP::getNextByte();
	encoders.byte[1] = OrangutanSVP::getNextByte();

	// Read the total number of counts seen on CD (encoders.countCD).
	encoders.byte[2] = OrangutanSVP::getNextByte();
	encoders.byte[3] = OrangutanSVP::getNextByte();

	// Read the status flags.  When these flags are read, the auxiliary
	// processor clears the status byte, so we must use |= here to
	// preserve the status flags we have stored in status until the
	// user has seen them.
	encoders.status |= OrangutanSVP::getNextByte();

	return encoders;
}

// Issues the Read Port Bytes command and returns the number of bytes there are to read.
// Those bytes should then be read with calls to getNextByte().
unsigned char OrangutanSVP::serialReadStart()
{
    OrangutanSPIMaster::transmitAndDelay(0x83, 7);
	return getNextByte();
}

unsigned char OrangutanSVP::serialSendIfReady(char byte)
{
    OrangutanSPIMaster::transmitAndDelay(byte & 0x80 ? 0x85 : 0x84, 5);
	return OrangutanSPIMaster::transmitAndDelay(byte & 0x7F, 5);
}

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
	static unsigned long svp_variables_last_update_ms = 0xFFFFFFFF;
	
	if (OrangutanTime::ms() != svp_variables_last_update_ms)
	{
		updateVariables();
		svp_variables_last_update_ms = OrangutanTime::ms();
	}
}

static void updateEncodersIfNeeded()
{
	static unsigned long encoders_last_update_ms = 0xFFFFFFFF;
	
	if (OrangutanTime::ms() != encoders_last_update_ms)
	{
		updateEncoders();
		encoders_last_update_ms = OrangutanTime::ms();
	}
}

unsigned int OrangutanSVP::getTrimpotMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.trimpot;
}

unsigned int OrangutanSVP::getBatteryMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.battery;
}

unsigned int OrangutanSVP::getChannelAMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.channelA;
}

unsigned int OrangutanSVP::getChannelBMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.channelB;
}

unsigned int OrangutanSVP::getChannelCMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.channelC;
}

unsigned int OrangutanSVP::getChannelDMillivolts()
{
	updateVariablesIfNeeded();
	return svp_variables.channelD;
}

SVPStatus OrangutanSVP::getStatus()
{
	updateVariablesIfNeeded();
	return svp_variables.status;
}

int OrangutanSVP::getCountsAB()
{
	updateEncodersIfNeeded();
	return encoders.countAB - lastCountAB;
}

int OrangutanSVP::getCountsAndResetAB()
{
	int temp = getCountsAB();
	lastCountAB = encoders.countAB;
	return temp;
}

int OrangutanSVP::getCountsCD()
{
	updateEncodersIfNeeded();
	return encoders.countCD - lastCountCD;
}

int OrangutanSVP::getCountsAndResetCD()
{
	int temp = getCountsCD();
	lastCountCD = encoders.countCD;
	return temp;
}

unsigned char OrangutanSVP::checkErrorAB()
{
	updateEncodersIfNeeded();
	if (encoders.errorAB)
	{
		encoders.errorAB = 0;
		return 1;
	}
	return 0;
}

unsigned char OrangutanSVP::checkErrorCD()
{
	updateEncodersIfNeeded();
	if (encoders.errorCD)
	{
		encoders.errorCD = 0;
		return 1;
	}
	return 0;
}

#endif
