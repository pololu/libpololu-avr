#ifndef PololuQTRSensors_h
#define PololuQTRSensors_h

#define QTR_EMITTERS_OFF 0
#define QTR_EMITTERS_ON 1
#define QTR_EMITTERS_ON_AND_OFF 2

char qtr_rc_init(unsigned char* pins, unsigned char numSensors, 
		 unsigned int timeout, unsigned char emitterPin);
char qtr_analog_init(unsigned char* analogPins, unsigned char numSensors, 
		     unsigned char numSamplesPerSensor, unsigned char emitterPin);
void qtr_emitters_on();
void qtr_emitters_off();
void qtr_read(unsigned int *sensor_values, unsigned char readMode);
void qtr_calibrate(unsigned char readMode);
void qtr_reset_calibration();
void qtr_read_calibrated(unsigned int *sensor_values, unsigned char readMode);
unsigned int qtr_read_line(unsigned int *sensor_values, unsigned char readMode);
unsigned int qtr_read_line_white(unsigned int *sensor_values, unsigned char readMode);

unsigned int *qtr_calibrated_minimum_on();
unsigned int *qtr_calibrated_maximum_on();
unsigned int *qtr_calibrated_minimum_off();
unsigned int *qtr_calibrated_maximum_off();

#endif