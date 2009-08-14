/*
  3pi.h - Library for using the 3pi robot.
*/

/*
 * Written by Paul Grayson, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18/6
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

#ifndef Pololu3pi_h
#define Pololu3pi_h

#include <pololu/analog.h>
#include <pololu/buzzer.h>
#include <pololu/time.h>
#include <pololu/motors.h>
#include <pololu/lcd.h>
#include <pololu/leds.h>
#include <pololu/pushbuttons.h>
#include <pololu/serial.h>

#define IR_EMITTERS_OFF 0
#define IR_EMITTERS_ON 1
#define IR_EMITTERS_ON_AND_OFF 2

void pololu_3pi_init(unsigned int line_sensor_timeout);
void pololu_3pi_init_disable_emitter_pin(unsigned int line_sensor_timeout);
void read_line_sensors(unsigned int *sensor_values, unsigned char readMode);
void emitters_on();
void emitters_off();
void calibrate_line_sensors(unsigned char readMode);
void line_sensors_reset_calibration();
void read_line_sensors_calibrated(unsigned int *sensor_values, unsigned char readMode);
unsigned int read_line(unsigned int *sensor_values, unsigned char readMode);
unsigned int read_line_white(unsigned int *sensor_values, unsigned char readMode);

unsigned int *get_line_sensors_calibrated_minimum_on();
unsigned int *get_line_sensors_calibrated_maximum_on();
unsigned int *get_line_sensors_calibrated_minimum_off();
unsigned int *get_line_sensors_calibrated_maximum_off();

#endif
