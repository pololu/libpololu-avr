void qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
		 unsigned int timeout_us, unsigned char emitterPin);
void qtr_emitters_on();
void qtr_emitters_off();
void read_line_sensors(unsigned int *sensor_values);
