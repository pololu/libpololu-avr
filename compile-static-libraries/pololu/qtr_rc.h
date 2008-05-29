void qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
		 unsigned int timeout_us, unsigned char emitterPin);
void qtr_emitters_on();
void qtr_emitters_off();
void qtr_read(unsigned int *sensor_values);
void qtr_calibrate();
void qtr_read_calibrated(unsigned int *sensor_values);
unsigned int qtr_read_line(unsigned int *sensor_values, bool white_line);
