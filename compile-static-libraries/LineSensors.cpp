#define F_CPU 20000000UL
#include <util/delay.h>
#include <avr/io.h>

#include "LineSensors.h"

#ifdef LIB_ORANGUTAN
extern "C" void ir_leds_on()
{
  OrangutanLineSensors::emittersOn();
}

extern "C" void ir_leds_off()
{
  OrangutanLineSensors::emittersOff();
}

// returns 5 raw RC sensor values
extern "C" void read_line_sensors(unsigned int *sensor_values) {
  OrangutanLineSensors::read(sensor_values);
}
#endif

OrangutanLineSensors::OrangutanLineSensors()
{
  // no initialization needed
}

void OrangutanLineSensors::read(unsigned int *sensor_values)
{
  uint8_t last_value = 0xff;
  uint8_t i;
  int count;

  // reset the values
  for(i=0;i<5;i++)
    sensor_values[i] = 0;

  // set all ports to outputs
  DDRC |= 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4;

  // drive high for 10 us
  PORTC |= 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4;

  _delay_us(10);

  emittersOn();

  // set all ports to inputs
  DDRC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  // turn off pull ups
  PORTC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  for( count = 0; count < 1000; count ++ )
  {
    uint8_t value = PINC & 0b00111111;
    i=0;

    if(value == last_value) continue;

    last_value = value;

    for(i=0;i<5;i++) {
      if(!sensor_values[i] && !(value & 1))
	sensor_values[i] = count;
      value >>= 1;
    }
    
    _delay_us(1);
  }

  DDRC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  emittersOff();

  for(i=0;i<5;i++)
    if(!sensor_values[i])
      sensor_values[i] = 1000;

}

void OrangutanLineSensors::emittersOff()
{
  // set to output
  DDRC |= 1<<5;
  PORTC &= ~(1<<5);
}

void OrangutanLineSensors::emittersOn()
{
  // set to output
  DDRC |= 1<<5;
  PORTC |= 1<<5;
}
