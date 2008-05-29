void qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
		 unsigned int timeout_us, unsigned char emitterPin);
void ir_leds_on();
void ir_leds_off();
void read_line_sensors(unsigned int *sensor_values);
