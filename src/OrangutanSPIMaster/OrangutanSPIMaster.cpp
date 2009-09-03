/*
  OrangutanSPIMaster.cpp - Library for communicating using the AVR's hardware
      SPI (Serial Peripheral Interface) module in master mode.
*/

/*
 * Written by David Grayson, Sep 2, 2009.
 * Copyright (c) 2009 Pololu Corporation. For more information, see
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

#include "avr/io.h"
#include "OrangutanSPIMaster.h"
#include "../include/atmega328p_pins.h"

#ifdef LIB_POLOLU
#include "../OrangutanTime/OrangutanTime.h" // provides delayMicroseconds()
#else
#include "wiring.h"		// provides delayMicroseconds()
#endif

// Default parameter values for svp_master_init

#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega1284P__)

// For the Orangutan SVP, these are the optimal settings for
// communication with the auxiliary processor:
#define SPI_SPEED_DIVIDER_DEFAULT SPI_SPEED_DIVIDER_4  // 20MHz / 4 = 5 MHz
#define SPI_EDGE_DEFAULT          SPI_EDGE_TRAILING

#else

// By default, pick the slowest possible speed.
#define SPI_SPEED_DIVIDER_DEFAULT SPI_SPEED_DIVIDER_128  // 20MHz / 128 = 156 kHz
#define SPI_EDGE_DEFAULT          SPI_EDGE_LEADING

#endif

// C wrapper functions
#ifdef LIB_POLOLU

extern "C" void spi_master_init(unsigned char sampling_edge, unsigned char speed_divider)
{
	OrangutanSPIMaster::init(sampling_edge, speed_divider);
}

extern "C" unsigned char spi_master_transmit(unsigned char data)
{
	return OrangutanSPIMaster::transmit(data);
}

extern "C" unsigned char spi_master_transmit_and_delay(unsigned char data, unsigned char delay)
{
	return OrangutanSPIMaster::transmitAndDelay(data, delay);
}

#endif

/*  OrangutanSPIMaster::init(unsigned char sampling_edge, unsigned char speed_divider);
    spi_master_init(unsigned char sampling_edge, unsigned char speed_divider);

    Initializes the AVR's hardware SPI module in master mode.

	Most users do not need to call this function explicitly, because any function
	that uses the SPI module in master mode will call this function automatically
	with default parameters if the SPI module has not been enabled yet.

    This function makes the MOSI (PB5) and SCK (PB7) pins outputs so that the
	AVR can send data to the slave device.  This function makes MISO (PB6) an
	input so that the AVR can receive data from the slave device.

	sampling_edge specifies which SCK edge the AVR will sample data on.
	    SPI_EDGE_LEADING - The AVR will take a digital reading of MISO
		    on the leading (rising) edge of SCK.
		SPI_EDGE_TRAILING - The AVR will take a digital reading of MISO
		    on the trailing (falling) edge of SCK.

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

	The AVR's SPI module is designed so that if PB4 is an input and it reads
	low (0 V), then the SPI module will automatically go in to slave mode.
	Therefore, it is recommended to make PB4 an output before doing SPI master
	communication.  If PB4 be an output, then the SPI inititialization routine
	in this library will enable the pull-up resistor on that line.

	If SSbar (PB4) is an input and it goes low while the SPI module is in
	master mode, then the MSTR bit in SPCR will become zero, and all SPI
	transmission functions in this library will return a result of zero.  If you
	are having trouble with SPI	communication, try making SSbar be an output by
	inserting one of these lines at the beginning of your program:

	    set_digital_output(IO_B4, LOW);

    or

	    set_digital_output(IO_B4, HIGH);

	or

		DDRB |= 1<<DDB4;

	*/
void OrangutanSPIMaster::init(unsigned char sampling_edge, unsigned char speed_divider)
{
	// If SSbar (PB4) is an input...
	if ( !(DDRB & (1<<DDB4)) )
	{
	    // Enable the pull-up on SSbar (PB4).
		PORTB |= 1<<PORTB4;

		// Delay a while to give the pull-up time to make PB4 go high.
		// Otherwise, the SPI module might exit form master mode right away.
		delayMicroseconds(30);
	}

    // Make the MOSI and SCK pins outputs.
    DDRB |= (1<<PB5) | (1<<PB7);

    // Make sure the MISO pin is an input.
    DDRB &= ~(1<<PB6);
  
    // Set up the SPI module.
	SPCR = (1<<SPE) | (1<<MSTR) | (sampling_edge==SPI_EDGE_TRAILING ? (1<<CPHA) : 0) | (speed_divider & 3);  
	SPSR = (speed_divider & 4) ? 1 : 0;

	// Delay for a millisecond to give the slave device time to adjust.
	// This is necessary for the Orangutan SVP because the SCK line does not have
	// a pull-down, and the PIC's SPI module can not be enabled until SCK goes low.
	delayMicroseconds(1000);
}

/*  OrangutanSPIMaster::transmit(unsigned char data);
    spi_master_transmit(unsigned char data);

    Transmits the given byte of data to the SPI slave device, and returns the
	byte that the slave simultaneously sent back.

	This function represents the basic building block of SPI master
	communication.  All functions that perform SPI master communication
	in the Pololu AVR Library use this function.

    SPI is a synchronous communication protocol where the basic transaction
	consists of the master toggling the SCK line 8 times while bytes are
	simultaneously exchanged between the slave and the master.  There is
	no way for the master to send a byte without receiving one, and there
	is no way for the master to receive a byte without sending one.
 */
unsigned char OrangutanSPIMaster::transmit(unsigned char data)
{
	return transmitAndDelay(data, 0);
}

/*  OrangutanSPIMaster::transmitAndDelay(unsigned char data, unsigned char post_delay_us);
    spi_master_transmit_and_delay(unsigned char data, unsigned char post_delay_us);

	This function is just like svp_master_transmit except that after the transmission
	has finished it delays for the specified number of microseconds before returning.

	This added delay is useful if you communicating with any slave device that
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
		OrangutanSPIMaster::init(SPI_EDGE_DEFAULT, SPI_SPEED_DIVIDER_DEFAULT);
	}
	
	if (!(SPCR&(1<<MSTR)))
	{
		// The SPI module is enabled, but it is in slave mode, so we can not
		// transmit the byte.  This can happen if SSbar (PB4) is an input and
		// it went low.  See the comment for svp_master_init for more info.
		return 0;
	}

	// Begin transmission.
	SPDR = data;

	// Wait for the transmission to finish.
	while(!(SPSR & (1<<SPIF))){};

	// Do the post-delay.
	while(post_delay_us)
	{
		delayMicroseconds(1);
		post_delay_us--;
	}
	//delayMicroseconds(post_delay_us); // TODO: make this code work!

	// Return the byte that the slave device sent during that transmission.
	return SPDR;
}
