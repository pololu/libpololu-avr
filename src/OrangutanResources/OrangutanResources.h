/*
  OrangutanResources.h - Measures available RAM on the AVR
*/

/*
 * Written by Paul Grayson, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
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
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
