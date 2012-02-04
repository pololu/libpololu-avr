/*
  OrangutanResources.cpp - Measures available RAM on the AVR
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

#include "OrangutanResources.h"


extern "C" int get_free_ram()
{
	return OrangutanResources::getFreeRAM();
}


// constructor

OrangutanResources::OrangutanResources()
{
}

extern int __bss_end; // the top of static variable memory
extern void *__brkval; // the top of memory used by malloc()

int OrangutanResources::getFreeRAM()
{
	// A local variable gives us the location of the
	// stack pointer:
	int free_memory;

	if((int)__brkval == 0) // if malloc is not in use
		free_memory = ((int)&free_memory) - ((int)&__bss_end);
	else
		free_memory = ((int)&free_memory) - ((int)__brkval);

	return free_memory;
}


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
