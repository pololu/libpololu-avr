#define MODE_8_BIT		1
#define MODE_10_BIT		0
#define TRIMPOT			7
#define TEMP_SENSOR		6

void set_analog_mode(unsigned char mode);
unsigned char get_analog_mode();
unsigned int analog_read(unsigned char channel);
unsigned int analog_read_average(unsigned char channel, unsigned int samples);
unsigned int read_trimpot();

int read_temperature_f();
int read_temperature_c();

void start_analog_conversion(unsigned char channel);
unsigned char analog_is_converting();
unsigned int analog_conversion_result();
unsigned int to_millivolts(unsigned int analog_result);

unsigned int read_battery_millivolts_3pi();
static inline unsigned int read_battery_millivolts()
{
  return read_battery_millivolts_3pi();
}
unsigned int read_battery_millivolts_sv168();

