#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#define MODE_8_BIT		1
#define MODE_10_BIT		0
#define TRIMPOT			7
#define TEMP_SENSOR		6

void set_analog_mode(unsigned char mode);
unsigned char get_analog_mode();
unsigned int analog_read(unsigned char channel);
unsigned int analog_read_average(unsigned char channel, unsigned int samples);

void start_analog_conversion(unsigned char channel);
void start_analog_conversion_using_internal_reference(unsigned char channel);
unsigned char analog_is_converting();
unsigned int analog_conversion_result();
unsigned int to_millivolts(unsigned int analog_result);

#if !defined (__AVR_ATmega324P__) || !defined (__AVR_ATmega1284P__)

unsigned int read_trimpot();

int read_temperature_f();
int read_temperature_c();

unsigned int read_battery_millivolts_3pi();
static inline unsigned int read_battery_millivolts()
{
  return read_battery_millivolts_3pi();
}
unsigned int read_battery_millivolts_sv();
unsigned int read_battery_millivolts_sv168();

#endif

#endif
