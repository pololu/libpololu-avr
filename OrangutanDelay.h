#ifndef OrangutanDelay_h
#define OrangutanDelay_h

class OrangutanDelay
{
  public:
	// Delays the specified number of milliseconds, which can be any
	// integer value.
	static void ms(unsigned int milliseconds);

	// Delays the specified number of microseconds, which can be any
	// integer value.  This only works at 20MHz.
	static inline void us(unsigned int microseconds)
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
};

#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
