/*
  svp.h - Library of functions for the Orangutan SVP 324 and 1284,
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

#ifndef OrangutanSVP_h
#define OrangutanSVP_h

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)

#define SVP_MODE_RX           0
#define SVP_MODE_ANALOG       2
#define SVP_MODE_ENCODERS     4
#define SVP_MODE_SLAVE_SELECT 1

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
	};
} SVPStatus;

void svp_set_mode(unsigned char mode);
unsigned char svp_read_firmware_version();
SVPEncoders svp_read_encoders();
SVPStatus svp_read_status();
static inline unsigned char usb_power_present(){ return svp_read_status().usbPowerPresent; }
static inline unsigned char usb_configured(){ return svp_read_status().usbConfigured; }
static inline unsigned char usb_suspend(){ return svp_read_status().usbSuspend; }
static inline unsigned char dtr_enabled(){ return svp_read_status().dtrEnabled; }
static inline unsigned char rts_enabled(){ return svp_read_status().rtsEnabled; }

#endif

#endif