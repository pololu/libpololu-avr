/*
  OrangutanDelay.cpp - Library for delay, calibrated for Orangutan
*/

#define F_CPU 20000000UL
#include <util/delay.h>	// F_CPU is defined in "device.h" above
#include "OrangutanDelay.h"

// delay for time_ms milliseconds by looping
extern "C" void delay_ms(unsigned int milliseconds)
{
  OrangutanDelay::ms(milliseconds);
}

void OrangutanDelay::ms(unsigned int milliseconds)
{
  unsigned int i;

  // _delay_ms() comes from <util/delay.h>
  //  and can only delay for a max of around 13 ms
  //  when the IO clock is 20 MHz.
  for ( i = 0; i < milliseconds; i++ )
    _delay_ms( 1 );
}
