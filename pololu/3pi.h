#include <pololu/analog.h>
#include <pololu/buzzer.h>
#include <pololu/time.h>
#include <pololu/motors.h>
#include <pololu/lcd.h>
#include <pololu/leds.h>
#include <pololu/pushbuttons.h>

#define BUTTON_C	(1 << 5)		// Arduino pin 13
#define BUTTON_B	(1 << 4)		// Arduino pin 12
#define BUTTON_A	(1 << 1)		// Arduino pin 9

#define IR_EMITTERS_OFF 0
#define IR_EMITTERS_ON 1
#define IR_EMITTERS_ON_AND_OFF 2

void pololu_3pi_init(unsigned int line_sensor_timeout_us);
unsigned int battery_millivolts();
void read_line_sensors(unsigned int *sensor_values, unsigned char readMode);
void emitters_on();
void emitters_off();
void calibrate_line_sensors(unsigned char readMode);
void read_line_sensors_calibrated(unsigned int *sensor_values, unsigned char readMode);
unsigned int read_line(unsigned int *sensor_values, unsigned char readMode);
unsigned int read_line_white(unsigned int *sensor_values, unsigned char readMode);
