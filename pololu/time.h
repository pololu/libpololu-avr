#ifndef OrangutanTime_h
#define OrangutanTime_h

// these are defined in the .cpp file:
unsigned long get_ms();
void delay_ms(unsigned int milliseconds);
void time_reset();

// This is inline for efficiency:
static inline void delay_us(unsigned int microseconds)
{
  __asm__ volatile (
		    "1: push r22"     "\n\t"
		    "   ldi  r22, 4"  "\n\t"
		    "2: dec  r22"     "\n\t"
		    "   brne 2b"      "\n\t"
		    "   pop  r22"     "\n\t"   
		    "   sbiw %0, 1"   "\n\t"
		    "   brne 1b"
		    : "=w" ( microseconds )  
		    : "0" ( microseconds )
		    );  
}

// These are alternative aliases:
static inline void delay(unsigned int milliseconds) { delay_ms(milliseconds); }
static inline unsigned long millis() { return get_ms(); }
static inline void delayMicroseconds(unsigned int microseconds) { delay_us(microseconds); }

#endif