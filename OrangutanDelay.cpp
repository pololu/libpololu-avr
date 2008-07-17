/*
  OrangutanDelay.cpp - Library for delay, calibrated for Orangutan
*/

#define F_CPU 20000000UL
#include "OrangutanDelay.h"

extern "C" void delay_ms(unsigned int milliseconds)
{
  OrangutanDelay::ms(milliseconds);
}

void OrangutanDelay::ms(unsigned int milliseconds)
{
  unsigned int i;

  for ( i = 0; i < milliseconds; i++ )
    us( 1000 );
}
