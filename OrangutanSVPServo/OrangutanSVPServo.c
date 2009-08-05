#include <avr/io.h>
#include <avr/interrupt.h>


#define BUZZER		(1 << PORTD4)
#define M1PWM		(1 << PORTD7)
#define M1DIR		(1 << PORTC6)
#define M2PWM		(1 << PORTD6)
#define M2DIR		(1 << PORTC7)


volatile unsigned int servoPos[8]	= {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int servoTargetPos[8] 		= {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int servoSpeed[8]			= {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char idx;

struct PinStruct
{
	unsigned char* pinRegister;
	unsigned char bitmask;
};
struct PinStruct muxSelectors[3];


// This interrupt is executed when timer 1 = TOP (ICR1) and the value in OCR1B (the next duty cycle)
// has been loaded.
ISR(TIMER1_CAPT_vect)
{
	*(muxSelectors[0].pinRegister) ^= muxSelectors[0].bitmask;
	if (idx & 1)
	{
		*(muxSelectors[1].pinRegister) ^= muxSelectors[1].bitmask;
		if (idx & 2)
		{
			*(muxSelectors[2].pinRegister) ^= muxSelectors[2].bitmask;
		}
	}
	
	idx = (idx + 1) & 7;
	int pos = servoPos[idx];
	if (servoSpeed[idx])
	{
		if (servoTargetPos[idx] > pos)
		{
			pos += servoSpeed[idx];
			if (pos > servoTargetPos[idx])
				pos = servoTargetPos[idx];
		}
		else
		{
			if (pos < servoTargetPos[idx] + servoSpeed[idx])
				pos = servoTargetPos[idx];
			else
				pos -= servoSpeed[idx];
		}
	}
	else
	{
		pos = servoTargetPos[idx];
	}
	OCR1A = pos;
	servoPos[idx] = pos;	
}


int getServoPosition(unsigned char servoNum)
{
	return (servoPos[servoNum] + 5) / 10;
}


// send a position value of 0 to turn off the servo
void setServoTarget(unsigned char servoNum, unsigned int pos_us)
{
	if (pos_us > 2450)			// will get bad results if pulse is 100% duty cycle (2500)
		pos_us = 2450;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoTargetPos[servoNum & 7] = pos_us * 10;
	TIMSK1 |= 1 << ICIE1;
}


int getServoTarget(unsigned char servoNum)
{
	return (servoTargetPos[servoNum & 7] + 5) / 10;
}


// speed parameter is in units of 100ns (1/10th of a microsecond)
// the servo position will be incremented or decremented by "speed"
// every 20 ms.
void setServoSpeed(unsigned char servoNum, unsigned int speed)
{
	if (speed > 25000)
		speed = 25000;
	TIMSK1 &= ~(1 << ICIE1);	// make sure we don't get interrupted in the middle of an update	
	servoSpeed[servoNum & 7] = speed;
	TIMSK1 |= 1 << ICIE1;
}


int getServoSpeed(unsigned char servoNum)
{
	return servoSpeed[servoNum & 7];
}

struct IOStruct
{
	unsigned char* pinRegister;
	unsigned char* portRegister;
	unsigned char* ddrRegister;
	unsigned char bitmask;
};


struct IOStruct getIORegisters(unsigned char pin)
{
	struct IOStruct io;

	io.pinRegister = 0;
	io.portRegister = 0;
	io.ddrRegister = 0;
	io.bitmask = 0;

	if (pin < 8)
	{
		io.pinRegister = (unsigned char*)&PIND;
		io.portRegister = (unsigned char*)&PORTD;
		io.ddrRegister = (unsigned char*)&DDRD;
		io.bitmask = 1 << pin;
	}
	else if (pin < 16)
	{
		io.pinRegister = (unsigned char*)&PINB;
		io.portRegister = (unsigned char*)&PORTB;
		io.ddrRegister = (unsigned char*)&DDRB;
		io.bitmask = 1 << (pin - 8);
	}
	else if (pin < 24)
	{
		io.pinRegister = (unsigned char*)&PINC;
		io.portRegister = (unsigned char*)&PORTC;
		io.ddrRegister = (unsigned char*)&DDRC;
		io.bitmask = 1 << (pin - 16);
	}
	else if (pin < 32)
	{
		io.pinRegister = (unsigned char*)&PINA;
		io.portRegister = (unsigned char*)&PORTA;
		io.ddrRegister = (unsigned char*)&DDRA;
		io.bitmask = 1 << (31 - pin);
	}

	return io;
}




void initMuxSelector(unsigned char selector, unsigned char pin)
{
	struct IOStruct io = getIORegisters(pin);
	*(io.portRegister) &= ~io.bitmask;			// PORT pin -> low output or high-impedance input
	*(io.ddrRegister) |= io.bitmask;			// pin -> output
	muxSelectors[selector].pinRegister = io.pinRegister;
	muxSelectors[selector].bitmask = io.bitmask;
}


void initServos(unsigned char SA, unsigned char SB, unsigned char SC)
{
	initMuxSelector(0, SA);
	initMuxSelector(1, SB);
	initMuxSelector(2, SC);
	
	idx = 0;

	PORTD &= ~(1 << PORTD5);	// MUX output pin -> low output or high-impedance input
	DDRD |= 1 << PORTD5;		// pin -> output

	TCCR1A = 0b10000010;		// clear OC1A on comp match when upcounting, set OC1A on comp match when downcounting
	TCCR1B = 0b00010001;		// phase correct PWM with TOP = ICR1, clock prescaler = 1 (freq = FCPU / (2 * ICR1))

	ICR1 = 25000;				// 400 Hz PWM (2.5 ms period)
	TIMSK1 |= (1 << ICIE1);		// enable T1 input capture interrupt (occurs at TOP, when buffered duty cycle is loaded)
	sei();
}



int main()
{
/*	DDRD |= BUZZER;			// needed while no pulldown on MOSFET line
	PORTD &= ~BUZZER;		// needed while no pulldown on MOSFET line
	DDRD |= 1 << PORTD5;
	DDRD |= (1 << PORTD0) | (1 << PORTD1) | (1 << PORTD2);


	TCCR1A = 0b10000010;	// clear OC1A on comp match when upcounting, set OC1A on comp match when downcounting
	TCCR1B = 0b00010001;	// phase correct PWM with TOP = ICR1, clock prescaler = 1 (freq = FCPU / (2 * ICR1))

	ICR1 = 25000;	// 400 Hz PWM (2.5 ms period)
	TIMSK1 |= (1 << ICIE1);
	sei();

	int i;
	for (i = 0; i < 8; i++)
	{
		setServoTarget(i, 1000 + i * 200);
	}

	while (1)
	{
		setServoTarget(0, 1250 + i);
		setServoTarget(1, 1250 + i / 2);
		setServoTarget(2, 1750 - i);
		setServoTarget(3, 1750 - i / 2);
		setServoTarget(4, 1250 + ((i * 2) & 511));
		setServoTarget(5, 1750 - ((i * 2) & 511));
		setServoTarget(6, 1250 + ((i * 3) & 511));
		setServoTarget(7, 1750 - ((i * 3) & 511));
		//delay_ms(2);
		i = (i + 1) & 511;
	}*/
}


/*
int main()
{
	PORTC |= 1 << PORTC5;	// enable pull up on PC5
	DDRD |= BUZZER;
	PORTD &= ~BUZZER;

	DDRD |= M1PWM | M2PWM;
	DDRC |= M1DIR | M2DIR;

	TCCR2A = 0b10100001;  	// bits 4-7: clear OC2x on compare match when upcounting, set OC2x on match when downcounting
							// bits 0-1: phase correct PWM with TOP=0xFF (with bit 3 clear in TCCR2B)
	TCCR2B = 0b00000001;	// clock prescaler = 1 (20 MHz / 2 / 255) = 39.2 kHz PWM)

	OCR2A = 0;
	OCR2B = 0;

	//PORTD |= M2PWM | M1PWM;
	//PORTC |= M1DIR | M2DIR;

	unsigned char i = 0;
	unsigned char motor = 0;
	unsigned char dir = 0;

	while (PINC & (1 << PORTC5))
		;

	while (1)
	{
		i++;
		
		if (motor)
		{
			OCR2B = i;
			if (dir)
				PORTC |= M2DIR;
			else
				PORTC &= ~M2DIR;
		}
		else
		{
			OCR2A = i;
			if (dir)
				PORTC |= M1DIR;
			else
				PORTC &= ~M1DIR;
		}

		if (!i)
		{
			motor = !motor;
			if (motor)
				dir = !dir;
		}
		//if (i == 0xFF)
			//delay_ms(500);

		delay_ms(5);
	}
}*/


 

