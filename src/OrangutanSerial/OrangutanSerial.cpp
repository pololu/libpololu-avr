/*
  OrangutanSerial.cpp - Library for serial transmit and receive.
*/

/*
 * Written by Paul Grayson et al., October, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include "OrangutanSerial.h"
#include "../OrangutanTime/OrangutanTime.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef LIB_POLOLU

extern "C" void serial_set_mode(unsigned char mode)
{
	OrangutanSerial::setMode(mode);
}

extern "C" void serial_check()
{
	OrangutanSerial::check();
}

extern "C" void serial_set_baud_rate(unsigned long baud)
{
	OrangutanSerial::setBaudRate(baud);
}

extern "C" void serial_receive(char *buffer, unsigned char size)
{
	OrangutanSerial::receive(buffer, size);
}

extern "C" char serial_receive_blocking(char *buffer, unsigned char size, unsigned int timeout_ms)
{
	return OrangutanSerial::receiveBlocking(buffer, size, timeout_ms);
}

extern "C" void serial_receive_ring(char *buffer, unsigned char size)
{
	OrangutanSerial::receiveRing(buffer, size);
}

extern "C" void serial_cancel_receive()
{
	OrangutanSerial::cancelReceive();
}

extern "C" unsigned char serial_get_received_bytes()
{
	return OrangutanSerial::getReceivedBytes();
}

extern "C" char serial_receive_buffer_full()
{
	return OrangutanSerial::receiveBufferFull();
}

extern "C" void serial_send(char *buffer, unsigned char size)
{
	OrangutanSerial::send(buffer, size);
}

extern "C" void serial_send_blocking(char *buffer, unsigned char size)
{
	OrangutanSerial::sendBlocking(buffer, size);
}

extern "C" unsigned char serial_get_sent_bytes()
{
	return OrangutanSerial::getSentBytes();
}

extern "C" char serial_send_buffer_empty()
{
	return OrangutanSerial::sendBufferEmpty();
}

#endif

volatile unsigned char OrangutanSerial::sentBytes;
volatile unsigned char OrangutanSerial::receivedBytes;
unsigned char OrangutanSerial::sendSize;
unsigned char OrangutanSerial::receiveSize;
unsigned char OrangutanSerial::receiveRingOn;

char *OrangutanSerial::sendBuffer;
char *OrangutanSerial::receiveBuffer;

// Inner function to be called by the ISR and by serial_check
inline void serial_rx()
{
	// It's important to read the byte out of the register, or else
	// the interrupt will be called again right after it executes.
	char a = UDR0;
	if(OrangutanSerial::receiveBuffer && OrangutanSerial::receivedBytes < OrangutanSerial::receiveSize)
	{
		OrangutanSerial::receiveBuffer[OrangutanSerial::receivedBytes] = a;
		OrangutanSerial::receivedBytes++; // the byte has been received
	}
	if(OrangutanSerial::receivedBytes == OrangutanSerial::receiveSize &&
	   OrangutanSerial::receiveRingOn)
		OrangutanSerial::receivedBytes = 0; // reset the ring
}

// Inner function to be called by the ISR and by serial_check
inline void serial_tx()
{
	OrangutanSerial::sentBytes ++; // we started sending a byte

	if(OrangutanSerial::sendBuffer && OrangutanSerial::sentBytes < OrangutanSerial::sendSize)
	{
		UDR0 = OrangutanSerial::sendBuffer[OrangutanSerial::sentBytes];
	}
}

void OrangutanSerial::init()
{
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	// Enable interrupts if desired
	if(mode == SERIAL_AUTOMATIC)
		UCSR0B |= (1<<RXCIE0)|(1<<TXCIE0);

	sei();
}

void OrangutanSerial::setBaudRate(unsigned long baud)
{
	init();

	unsigned int ubbr0 = F_CPU/(16*baud);

	UBRR0H = ubbr0 >> 8;
	UBRR0L = (unsigned char)ubbr0;
}

void OrangutanSerial::setMode(unsigned char m)
{
	mode = m;
	init(); // reset the interrupt status
}

void OrangutanSerial::check()
{
	if(UCSR0A & (1<<UDRE0)) // a byte has been sent
		serial_tx();
	if(UCSR0A & (1<<RXC0)) // a byte has been received
		serial_rx();
}

void OrangutanSerial::receive(char *buffer, unsigned char size)
{
	receiveBuffer = buffer;
	receivedBytes = 0;
	receiveSize = size;
	receiveRingOn = 0;
}

char OrangutanSerial::receiveBlocking(char *buffer, unsigned char size, unsigned int timeout)
{
	receive(buffer, size);

	unsigned long start_time = get_ms();
	while(!serial_receive_buffer_full() && (get_ms()-start_time) < timeout);

	if(!serial_receive_buffer_full())
		return 1;
	return 0;
}

void OrangutanSerial::receiveRing(char *buffer, unsigned char size)
{
	receive(buffer,size);
	receiveRingOn = 1;
}

void OrangutanSerial::cancelReceive()
{
	receive(0,0);
}

ISR(USART_RX_vect)
{
	serial_rx();
}

void OrangutanSerial::send(char *buffer, unsigned char size)
{
	sendBuffer = buffer;
	sentBytes = 0;
	sendSize = size;

	// send the first byte; the rest will be started by the ISR
	UDR0 = buffer[0];
}

void OrangutanSerial::sendBlocking(char *message, unsigned char size)
{
	OrangutanSerial::send(message, size);

	// wait for sending before returning
	while(!OrangutanSerial::sendBufferEmpty());
}

ISR(USART_UDRE_vect)
{
	serial_tx();
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
