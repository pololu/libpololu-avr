/*
  OrangutanResources.h - Measures available RAM on the AVR
*/

/*
 * Written by Paul Grayson, 2008.
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
 
#ifndef OrangutanResources_h
#define OrangutanResources_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../OrangutanResources/include/OrangutanModel.h"

#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
#define JTAG_RESET		(1 << JTRF)
#endif
#define WATCHDOG_RESET	(1 << WDRF)
#define BROWNOUT_RESET	(1 << BORF)
#define EXTERNAL_RESET	(1 << EXTRF)
#define POWERON_RESET	(1 << PORF)

#ifdef __cplusplus

class OrangutanResources
{
  public:

    // constructor (doesn't do anything)
	OrangutanResources();

	// Returns an estimate of the available free RAM on the AVR, in
	// bytes.  This is computed as the difference between the bottom
	// of the stack and the top of the static variable space or the
	// top of the space used by malloc().
	static int getFreeRAM();
	
	// returns the register that contains latched flags indicating
	// previous reset sources.  Individual flags can be accessed by
	// ANDing the result with the x_RESET constants defined in this
	// file.  For example:
	// if (OrangutanResources::getResetFlags() & WATCHDOG_RESET) ..
	static inline unsigned char getResetFlags()
	{
		return MCUSR;
	}
	
	// clear latched reset flags
	static inline void clearResetFlags()
	{
		MCUSR = 0;
	}
};

extern "C" {
#endif // __cplusplus

int get_free_ram(void);

// returns the register that contains latched flags indicating
// previous reset sources.  Individual flags can be accessed by
// ANDing the result with the x_RESET constants defined in this
// file.  For example:
// if (get_reset_flags() & WATCHDOG_RESET) ..
static inline unsigned char get_reset_flags(void)
{
	return MCUSR;
}

// clear latched reset flags
static inline void clear_reset_flags(void)
{
	MCUSR = 0;
}

#ifdef __cplusplus
}
#endif

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
