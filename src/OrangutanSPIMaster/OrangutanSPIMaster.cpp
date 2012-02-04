/*
  OrangutanSPIMaster.cpp - Library for communicating using the AVR's hardware
      SPI (Serial Peripheral Interface) module in master mode.
*/

/*
 * Written by David Grayson, Sep 2, 2009.
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the three links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */

/* Changelog:
 * 2010-01-07: In transmitAndDelay, added a check to make sure that post_delay_us is
 *     non-zero before calling delayMicroseconds because delayMicroseconds does not
 *     work with an argument of zero.
 */


#include "avr/io.h"
#include "OrangutanSPIMaster.h"
#include "../OrangutanResources/include/OrangutanModel.h"
#include "../OrangutanTime/OrangutanTime.h" // provides delayMicroseconds()

// Default parameter values for svp_master_init

#ifdef _ORANGUTAN_SVP

#define PIN_SSBAR 4
#define PIN_MOSI  5
#define PIN_MISO  6
#define PIN_SCK   7

// For the Orangutan SVP, these are the optimal settings for
// communication with the auxiliary processor:
#define SPI_DEFAULT_SPEED_DIVIDER SPI_SPEED_DIVIDER_4  // 20MHz / 4 = 5 MHz
#define SPI_DEFAULT_OPTIONS       SPI_EDGE_TRAILING

#elif defined(_ORANGUTAN_X2)

#define PIN_SSBAR 4
#define PIN_MOSI  5
#define PIN_MISO  6
#define PIN_SCK   7

// For the Orangutan X2, these are the optimal settings for
// communication with the auxiliary processor:
#define SPI_DEFAULT_SPEED_DIVIDER SPI_SPEED_DIVIDER_8  // 20MHz / 8 = 2.5 MHz
#define SPI_DEFAULT_OPTIONS       SPI_EDGE_LEADING


#else

#define PIN_SSBAR 2
#define PIN_MOSI  3
#define PIN_MISO  4
#define PIN_SCK   5

// By default, pick the slowest possible speed.
#define SPI_DEFAULT_SPEED_DIVIDER SPI_SPEED_DIVIDER_128  // 20MHz / 128 = 156 kHz
#define SPI_DEFAULT_OPTIONS       SPI_EDGE_LEADING

#endif

// C wrapper functions

extern "C" void spi_master_init(unsigned char speed_divider, unsigned char options)
{
	OrangutanSPIMaster::init(speed_divider, options);
}

extern "C" unsigned char spi_master_transmit(unsigned char data)
{
	return OrangutanSPIMaster::transmit(data);
}

extern "C" unsigned char spi_master_transmit_and_delay(unsigned char data, unsigned char delay)
{
	return OrangutanSPIMaster::transmitAndDelay(data, delay);
}


/*  OrangutanSPIMaster::init(unsigned char sampling_edge, unsigned char speed_divider);
    spi_master_init(unsigned char sampling_edge, unsigned char speed_divider);

    Initializes the AVR's hardware SPI module in master mode.

	Most users do not need to call this function explicitly, because any function
	that uses the SPI module in master mode will call this function automatically
	with default parameters if the SPI module has not been enabled yet.

    This function makes the MOSI and SCK pins outputs so that the
	AVR can send data to the slave device.  This function makes MISO an
	input so that the AVR can receive data from the slave device.

    speed_divider specifies the ratio of the AVR's clock frequency to the SPI
	    frequency.  If your AVR has a clock frequency of 20MHz, then the
		correspondence between speed_divider and SPI frequency is this:

		speed_divider         SPI Frequency (assuming 20 MHz clock)
		-----------------------------------
		SPI_SPEED_DIVIDER_2   10 MHz
		SPI_SPEED_DIVIDER_4   5 MHz
		SPI_SPEED_DIVIDER_8   2.5 MHz
		SPI_SPEED_DIVIDER_16  1.25 MHz
		SPI_SPEED_DIVIDER_32  625 kHz
		SPI_SPEED_DIVIDER_64  313 kHz
		SPI_SPEED_DIVIDER_128 156 kHz

	The AVR's SPI module is designed so that if SSbar is an input and it reads
	low (0 V), then the SPI module will automatically go in to slave mode.
	Therefore, it is recommended to make SSbar an output before doing SPI master
	communication.  If SSbar is an input, then the SPI inititialization routine
	in this library will enable the pull-up resistor on that line.

	If SSbar is an input and it goes low while the SPI module is in
	master mode, then the MSTR bit in SPCR will become zero, and all SPI
	transmission functions in this library will return a result of zero.  If you
	are having trouble with SPI	communication, try making SSbar be an output.
	*/
void OrangutanSPIMaster::init(unsigned char speed_divider, unsigned char options)
{
	// If SSbar is an input and it is not already pulled up...
	//if ( ~DDRB & ~PORTB & (1<<PIN_SSBAR) )
	if ( !(DDRB & (1<<PIN_SSBAR)) && !(PORTB & (1<<PIN_SSBAR)) )
	{
	    // Enable the pull-up on SSbar.
		PORTB |= 1<<PIN_SSBAR;

		// Delay a while to give the pull-up time to make SSbar go high.
		// Otherwise, the SPI module might exit form master mode right away.
		delayMicroseconds(30);
	}

    // Make the MOSI and SCK pins outputs.
    DDRB |= (1<<PIN_MOSI) | (1<<PIN_SCK);

    // Make sure the MISO pin is an input.
    DDRB &= ~(1<<PIN_MISO);
  
    // Set up the SPI module.
	SPCR = (1<<SPE) | (1<<MSTR) | (options & ~3) | (speed_divider & 3);  
	SPSR = (speed_divider & 4) ? 1 : 0;

	// Delay for a millisecond to give the slave device time to adjust.
	// This is necessary for the Orangutan SVP because the SCK line does not have
	// a pull-down, and the PIC's SPI module can not be enabled until SCK goes low.
	OrangutanTime::delayMilliseconds(1);
}

/*  OrangutanSPIMaster::transmit(unsigned char data);
    spi_master_transmit(unsigned char data);

    Transmits the given byte of data to the SPI slave device, and returns the
	byte that the slave simultaneously sent back.
 */
unsigned char OrangutanSPIMaster::transmit(unsigned char data)
{
	return transmitAndDelay(data, 0);
}

/*  OrangutanSPIMaster::transmitAndDelay(unsigned char data, unsigned char post_delay_us);
    spi_master_transmit_and_delay(unsigned char data, unsigned char post_delay_us);

	This function is just like svp_master_transmit except that after the transmission
	has finished it delays for the specified number of microseconds before returning.

	This added delay is useful if you are communicating with any slave device that
	requires some time between SPI transmissions to give it time to process the
	byte it has received (e.g. the auxiliary processors on the Orangutan X2 and
	Orangutan SVP).

	post_delay_us should be a number between 0 and 255.

	See svp_master_transmit for more information.
 */
unsigned char OrangutanSPIMaster::transmitAndDelay(unsigned char data, unsigned char post_delay_us)
{
	// If the SPI module has not been enabled yet, then enable it with default parameters.
	if ( !(SPCR & (1<<SPE)) )
	{
		init(SPI_DEFAULT_SPEED_DIVIDER, SPI_DEFAULT_OPTIONS);
	}
	
	if (!(SPCR&(1<<MSTR)))
	{
		// The SPI module is enabled, but it is in slave mode, so we can not
		// transmit the byte.  This can happen if SSbar is an input and
		// it went low.  See the comment for svp_master_init for more info.

		// We will try to recover by setting the MSTR bit.
		SPCR |= 1<<MSTR;
	}

	// Begin transmission.
	SPDR = data;

	// Wait for the transmission to finish.
	while(!(SPSR & (1<<SPIF)))
	{
		if (!(SPCR & (1<<MSTR)))
		{
			// The SPI module has left master mode, so return.
			// Otherwise, this will be an infinite loop.
			return 0;
		}
	}

	// Do the post delay.
	if (post_delay_us)
	{
		// This IF statement was added 2010-1-7 because delayMicroseconds does
		// not work with an argument of 0.
    	delayMicroseconds(post_delay_us);
	}

	// Return the byte that the slave device sent during that transmission.
	return SPDR;
}
