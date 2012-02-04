/*
  OrangutanSVP.h - Library of functions for the Orangutan SVP 324 and 1284,
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

#ifndef OrangutanSVP_h
#define OrangutanSVP_h

#include "../OrangutanResources/include/OrangutanModel.h"

#ifdef _ORANGUTAN_SVP

#define SVP_MODE_RX           0
#define SVP_MODE_ANALOG       2
#define SVP_MODE_ENCODERS     4

#define SVP_SLAVE_SELECT_ON   1

typedef	union SVPStatus
{
	unsigned char status;
	struct
	{
	    unsigned usbPowerPresent :1;
        unsigned usbConfigured :1;
		unsigned usbSuspend :1;
		unsigned dtrEnabled :1;
	    unsigned rtsEnabled :1;
		unsigned toggle :1;
	};
} SVPStatus;

#ifdef __cplusplus

// C++ Function Declarations

class OrangutanSVP
{
  public:
	static void setMode(unsigned char mode);
	static unsigned char getFirmwareVersion();

	// Status Functions
	static inline unsigned char usbPowerPresent(){ return getStatus().usbPowerPresent; }
	static inline unsigned char usbConfigured(){ return getStatus().usbConfigured; }
	static inline unsigned char usbSuspend(){ return getStatus().usbSuspend; }
	static inline unsigned char dtrEnabled(){ return getStatus().dtrEnabled; }
	static inline unsigned char rtsEnabled(){ return getStatus().rtsEnabled; }

	// Encoder Functions
	static int getCountsAB();
	static int getCountsCD();
	static int getCountsAndResetAB();
	static int getCountsAndResetCD();
	static unsigned char checkErrorAB();
	static unsigned char checkErrorCD();

	// Undocumented functions that are used by other parts of the library that
	// the typical user does not need to know about:
	static unsigned char serialSendIfReady(char data);
	static unsigned char getNextByte();
	static unsigned char serialReadStart();
	static unsigned int getBatteryMillivolts();
	static unsigned int getTrimpotMillivolts();
	static unsigned int getChannelAMillivolts();
	static unsigned int getChannelBMillivolts();
	static unsigned int getChannelCMillivolts();
	static unsigned int getChannelDMillivolts();
	static SVPStatus getStatus();
};

extern "C" {
#endif // __cplusplus

// C Function Declarations

void svp_set_mode(unsigned char mode);

unsigned char svp_get_firmware_version(void);

SVPStatus svp_get_status(void);
static inline unsigned char usb_power_present(void) { return svp_get_status().usbPowerPresent; }
static inline unsigned char usb_configured(void) { return svp_get_status().usbConfigured; }
static inline unsigned char usb_suspend(void) { return svp_get_status().usbSuspend; }
static inline unsigned char dtr_enabled(void) { return svp_get_status().dtrEnabled; }
static inline unsigned char rts_enabled(void) { return svp_get_status().rtsEnabled; }

// Encoder Functions
int svp_get_counts_ab(void);
int svp_get_counts_and_reset_ab(void);
int svp_get_counts_cd(void);
int svp_get_counts_and_reset_cd(void);
unsigned char svp_check_error_ab(void);
unsigned char svp_check_error_cd(void);

#ifdef __cplusplus
}
#endif

// endif 324 or 1284
#endif

#endif
