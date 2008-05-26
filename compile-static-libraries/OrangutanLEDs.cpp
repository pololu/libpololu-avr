#include <avr/io.h>

#define LED0 PD1
#define LED1 PD7

extern "C" void leds_init()
{
  // LEDs as an output
  DDRD |= 1 << LED0;
  DDRD |= 1 << LED1;
}

extern "C" void left_led(unsigned char on)
{
  if(on)
    PORTD |= 1 << LED0;
  else
    PORTD &= ~(1 << LED0);
}

extern "C" void right_led(unsigned char on)
{
  if(on)
    PORTD |= 1 << LED1;
  else
    PORTD &= ~(1 << LED1);
}

