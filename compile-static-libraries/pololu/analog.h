#define MODE_8_BIT		(1 << ADLAR)
#define MODE_10_BIT		0

void set_analog_mode(unsigned char mode);
unsigned char get_analog_mode();
unsigned int read_analog(unsigned char channel);
unsigned int read_analog_average(unsigned char channel, unsigned int samples);
unsigned int read_trimpot();
int read_temperature_f();
int read_temperature_c();
void start_analog_conversion(unsigned char channel);
unsigned char analog_is_converting();
unsigned int conversion_result();
unsigned int to_millivolts(unsigned int analog_result);
