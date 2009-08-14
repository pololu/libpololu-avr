#ifndef PololuWheelEncoders_h
#define PololuWheelEncoders_h

void encoders_init(unsigned char m1a, unsigned char m1b, unsigned char m2a, unsigned char m2b);
int encoders_get_counts_m1();
int encoders_get_counts_m2();
int encoders_get_counts_and_reset_m1();
int encoders_get_counts_and_reset_m2();
int encoders_check_error_m1();
int encoders_check_error_m2();

#endif