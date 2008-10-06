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

#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef LIB_POLOLU

extern "C" void serial_set_baud_rate(unsigned long baud)
{
	OrangutanSerial::setBaudRate(baud);
}

extern "C" void serial_receive(char *buffer, unsigned char size)
{
	OrangutanSerial::receive(buffer, size);
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

extern "C" unsigned char serial_get_sent_bytes()
{
	return OrangutanSerial::getSentBytes();
}

extern "C" char serial_send_buffer_empty()
{
	return OrangutanSerial::sendBufferEmpty();
}

#endif

unsigned char OrangutanSerial::sentBytes;
unsigned char OrangutanSerial::receivedBytes;
unsigned char OrangutanSerial::sendSize;
unsigned char OrangutanSerial::receiveSize;

char *OrangutanSerial::sendBuffer;
char *OrangutanSerial::receiveBuffer;

void OrangutanSerial::init()
{
	// Enable receiver and transmitter, and interrupts
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
	sei();
}

void OrangutanSerial::setBaudRate(unsigned long baud)
{
	init();

	unsigned int ubbr0 = F_CPU/(16*baud);

	UBRR0H = ubbr0 >> 8;
	UBRR0L = (unsigned char)ubbr0;
}

void OrangutanSerial::receive(char *buffer, unsigned char size)
{
	receiveBuffer = buffer;
	receivedBytes = 0;
	receiveSize = size;
}

ISR(SIG_USART_RECV)
{
	if(OrangutanSerial::receiveBuffer && OrangutanSerial::receivedBytes < OrangutanSerial::receiveSize)
	{
		OrangutanSerial::receiveBuffer[OrangutanSerial::receivedBytes] = UDR0;
		OrangutanSerial::receivedBytes++; // the byte has been received
	}
}

void OrangutanSerial::send(char *buffer, unsigned char size)
{
	sendBuffer = buffer;
	sentBytes = 0;
	sendSize = size;

	// send the first byte; the rest will be started by the ISR
	UDR0 = buffer[0];
}

ISR(SIG_USART_TRANS)
{
	OrangutanSerial::sentBytes ++; // we finished sending a byte

	if(OrangutanSerial::sendBuffer && OrangutanSerial::sentBytes < OrangutanSerial::sendSize)
	{
		UDR0 = OrangutanSerial::sendBuffer[OrangutanSerial::sentBytes];
	}
}

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
