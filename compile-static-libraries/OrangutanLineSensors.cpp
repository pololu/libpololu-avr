#define F_CPU 20000000UL
#include <util/delay.h>
#include <avr/io.h>

unsigned char singleSensor = 0;
unsigned int value[8];
unsigned int sum;
unsigned long weightedSum;

extern "C" void line_sensors_init() {
  DDRC = 0;
  //  TCCR1B = 0x02;	// use 20 MHz I/O clock divided by 8
  TCCR1B = 0x01;	// use 20 MHz I/O clock
}

extern "C" void ir_leds_on()
{
  // set to output
  DDRC |= 1<<5;
  PORTC |= 1<<5;
}

extern "C" void ir_leds_off()
{
  // set to output
  DDRC |= 1<<5;
  PORTC &= ~(1<<5);
}

// returns 5 raw RC sensor values
extern "C" void read_line_sensors(unsigned int *sensor_values) {
  uint8_t last_value = 0xff;
  uint8_t i;

  // reset the values
  for(i=0;i<5;i++)
    sensor_values[i] = 0;

  // set all ports to outputs
  DDRC |= 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4;

  // drive high for 10 us
  PORTC |= 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4;

  _delay_us(10);

  ir_leds_on();

  // set all ports to inputs
  DDRC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  // turn off pull ups
  PORTC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  TCNT1 = 0;
  while (TCNT1 < 20000) {
    uint8_t value = PINC & 0b00111111;
    i=0;

    if(value == last_value) continue;

    last_value = value;

    for(i=0;i<5;i++) {
      if(!sensor_values[i] && !(value & 1))
	sensor_values[i] = TCNT1;
      value >>= 1;
    }

  }

  DDRC &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4);

  ir_leds_off();

  for(i=0;i<5;i++)
    if(!sensor_values[i])
      sensor_values[i] = 20000;
}
