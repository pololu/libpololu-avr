/*
  OrangutanModel.h - Header file for determining which Orangutan model is
    being used.

	This header file tests for the __AVR_ATmega* macros and the
        _X2_1284 macro, and uses those to define the _ORANGUTAN_* macros,
        which are used when compiliing the library to compile different code
        for the different Orangutan models.

	This file is part of the internal implementation of the library; we
	do not recommend using the macros defined here in your code, because
	they may change.  You should instead copy the code in this file in to
	your own header file, and remove the leading underscore from the
	_ORANGUTAN_* macro names.
*/

/*
 * Written by David Grayson, Oct 21, 2010.
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

#undef _ORANGUTAN_X2   // Orangutan X2
#undef _ORANGUTAN_SVP  // Orangutan SVP-324 or 1284
#undef _ORANGUTAN_XX4  // Either the X2, SVP-324, or SVP-1284.

#if defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(_X2_1284)
 #define _ORANGUTAN_X2
#elif defined(__AVR_ATmega324P__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega1284PA__)
 #define _ORANGUTAN_SVP
#endif

#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
  #define _ORANGUTAN_XX4
#endif
