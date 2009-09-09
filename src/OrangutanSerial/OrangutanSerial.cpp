/*
  OrangutanSerial.cpp - Library for serial transmit and receive.
*/

/*
 * Written by Paul Grayson et al., October, 2008.
 * Copyright (c) 2008 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
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
#include "../OrangutanSVP/OrangutanSVP.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#ifdef LIB_POLOLU

extern "C" void serial_check()
{
	OrangutanSerial::check();
}

#if SERIAL_PORTS > 1

// C functions for devices with multiple serial ports.  A port argument is necessary.

extern "C" void serial_set_mode(unsigned char port, unsigned char mode)
{
	OrangutanSerial::setMode(port, mode);
}

extern "C" void serial_set_baud_rate(unsigned char port, unsigned long baud)
{
	OrangutanSerial::setBaudRate(port, baud);
}

extern "C" void serial_receive(unsigned char port, char *buffer, unsigned char size)
{
	OrangutanSerial::receive(port, buffer, size);
}

extern "C" char serial_receive_blocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout_ms)
{
	return OrangutanSerial::receiveBlocking(port, buffer, size, timeout_ms);
}

extern "C" void serial_receive_ring(unsigned char port, char *buffer, unsigned char size)
{
	OrangutanSerial::receiveRing(port, buffer, size);
}

extern "C" void serial_cancel_receive(unsigned char port)
{
	OrangutanSerial::cancelReceive(port);
}

extern "C" unsigned char serial_get_received_bytes(unsigned char port)
{
	return OrangutanSerial::getReceivedBytes(port);
}

extern "C" char serial_receive_buffer_full(unsigned char port)
{
	return OrangutanSerial::receiveBufferFull(port);
}

extern "C" void serial_send(unsigned char port, char *buffer, unsigned char size)
{
	OrangutanSerial::send(port, buffer, size);
}

extern "C" void serial_send_blocking(unsigned char port, char *buffer, unsigned char size)
{
	OrangutanSerial::sendBlocking(port, buffer, size);
}

extern "C" unsigned char serial_get_sent_bytes(unsigned char port)
{
	return OrangutanSerial::getSentBytes(port);
}

extern "C" char serial_send_buffer_empty(unsigned char port)
{
	return OrangutanSerial::sendBufferEmpty(port);
}

#else

// C Functions for devices with one UART.  No port argument is necessary.

extern "C" void serial_set_mode(unsigned char mode)
{
	OrangutanSerial::setMode(mode);
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

#endif

#if SERIAL_PORTS==1

// C++ Functions for devices with one UART.  No port argument is necessary.
// The functions called by these functions are inline (MULTIPORT_PUBLIC), so
// the compiler will know ahead of time that the port argument to those
// is going to be zero and it can make a lot of optimizations.

void OrangutanSerial::setBaudRate(unsigned long baud)
{
	setBaudRate(0, baud);
}

void OrangutanSerial::setMode(unsigned char mode)
{
	setMode(0, mode);
}

void OrangutanSerial::receive(char *buffer, unsigned char size)
{
	receive(0, buffer, size);
}

char OrangutanSerial::receiveBlocking(char *buffer, unsigned char size, unsigned int timeout_ms)
{
	return receiveBlocking(0, buffer, size, timeout_ms);
}

void OrangutanSerial::receiveRing(char *buffer, unsigned char size)
{
	receiveRing(0, buffer, size);
}

void OrangutanSerial::cancelReceive()
{
	cancelReceive(0);
}

void OrangutanSerial::send(char *buffer, unsigned char size)
{
	send(0, buffer, size);
}

void OrangutanSerial::sendBlocking(char *message, unsigned char size)
{
	sendBlocking(0, message, size);
}
#endif

/** VARIABLES *****************************************************************/

SerialPortData OrangutanSerial::ports[SERIAL_PORTS] =
{
	{0,0,0,0,0,0,0,0},
#if SERIAL_PORTS > 1
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
#endif
};

/** PRIVATE PROTOTYPES ********************************************************/
inline void uart_update_tx_interrupt(unsigned char port);
inline void serial_tx_check(unsigned char port);
inline void serial_rx_check(unsigned char port);

/** HELPERS *******************************************************************/

// UART-generic bit numbers.
#define RXEN RXEN0
#define TXEN TXEN0
#define RXCIE RXCIE0
#define RXEN RXEN0
#define TXEN TXEN0
#define UDRE UDRE0
#define RXC  RXC0

#if SERIAL_PORTS > 1
 #define PORT_ARGUMENT_VALUE port
#else
 #define PORT_ARGUMENT_VALUE 0
#endif

#define PORT_DATA ports[PORT_ARGUMENT_VALUE]

#if SERIAL_PORTS > 1

inline volatile unsigned char * ucsra(unsigned char port)
{
	if (port==0){ return &UCSR0A; }
	else { return &UCSR1A; }
}

inline volatile unsigned char * ucsrb(unsigned char port)
{
	if (port==0){ return &UCSR0B; }
	else { return &UCSR1B; }
}

inline volatile unsigned char * ucsrc(unsigned char port)
{
	if (port==0){ return &UCSR0C; }
	else { return &UCSR1C; }
}

inline volatile unsigned int * ubrr(unsigned char port)
{
	if (port==0){ return &UBRR0; }
	else { return &UBRR1; }
}

inline volatile unsigned char * udr(unsigned char port)
{
	if (port==0){ return &UDR0; }
	else { return &UDR1; }
}

#else

#define ucsra(port) (&UCSR0A)
#define ucsrb(port) (&UCSR0B)
#define ucsrc(port) (&UCSR0C)
#define ubrr(port) (&UBRR0)
#define udr(port) (&UDR0)

#endif

/** INITIALIZATION ************************************************************/

void OrangutanSerial::initMode()
{
	ports[0].mode = SERIAL_AUTOMATIC;
	#if defined(ORANGUTAN_SVP) || defined(ORANGUTAN_X2)
	ports[1].mode = SERIAL_AUTOMATIC;
	ports[2].mode = SERIAL_CHECK;
	#endif
}

inline void OrangutanSerial::initModeIfNeeded()
{
	static unsigned char initialized = 0;
	if (!initialized)
	{
		initialized = 1;
		initMode();
	}
}

// OPT: make this inline?
void OrangutanSerial::initPort(unsigned char port)
{
	initModeIfNeeded();

	#if SERIAL_PORTS > 1
	if (port == USB)
	{
		// No more initialization is required for the SPI/USB port.
		return;
	}
	#endif

	// Enable receiver and transmitter.
	// Enable reception interrupt if desired.
	if(PORT_DATA.mode == SERIAL_AUTOMATIC)
	{
		*ucsrb(port) = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);
	}
	else
	{
		*ucsrb(port) = (1<<RXEN) | (1<<TXEN);
	}

	// Enable transmission interrupt if desired.
	uart_update_tx_interrupt(PORT_ARGUMENT_VALUE);

	// Enable interrupts in general.
	sei();
}

MULTIPORT_PUBLIC void OrangutanSerial::setBaudRate(unsigned char port, unsigned long baud)
{
	initPort(port);

	#if SERIAL_PORTS > 1
	if (port == USB)
	{
		// You can't set the baud rate on the virtual COM port.
		return;
	}
	#endif

	// TODO: take out this temporarily variable if that is okay

	unsigned int brg = (F_CPU-8*baud)/(16*baud);

	*ubrr(port) = brg;
}

MULTIPORT_PUBLIC void OrangutanSerial::setMode(unsigned char port, unsigned char mode)
{
	initModeIfNeeded();

	ports[port].mode = mode;

	// reset the interrupt status
	initPort(port);
}

void OrangutanSerial::check()
{
	initModeIfNeeded();

	serial_tx_check(0);

#if SERIAL_PORTS > 1
	serial_tx_check(1);
#endif

#if SERIAL_PORTS > 2
	serial_tx_check(2);
#endif

	serial_rx_check(0);

#if SERIAL_PORTS > 1
	serial_rx_check(1);
#endif

#if SERIAL_PORTS > 2
	serial_rx_check(2);
#endif
}

/** RECEIVING *****************************************************************/

#ifdef ORANGUTAN_SVP
// The low-level SPI protocol for the Orangutan SVP's auxiliary processor only lets
// you receive bytes in chunks of unknown size.  When you ask to receive a byte,
// it tells you how many bytes are in the buffer, and then you HAVE to read all
// of them or else they will be lost.  Therefore, we implement this 8-byte buffer
// so the user can receive bytes at whatever pace he wants.
namespace OrangutanSVPRXBuffer
{
	static unsigned char byte_count;
	static unsigned char buffer[8];

	static unsigned char getReceivedBytes()
	{
		if (byte_count == 0)
		{
			byte_count = OrangutanSVP::serialReadStart();
			for (unsigned char i=0; i < byte_count; i++)
			{
				buffer[byte_count-1-i] = OrangutanSVP::readNextByte();
			}
		}
		return byte_count;
	}

	static unsigned char getNextByte()
	{
		byte_count--;
		return buffer[byte_count];
	}
}
#endif

// This is called in the main loop to take care of receiving bytes.
inline void OrangutanSerial::serial_rx_check(unsigned char port)
{
	#ifdef USB
	if (port==USB)
	{
		#ifdef ORANGUTAN_SVP

		// If we are trying to receive bytes, and a byte has been received...
		if(ports[port].receiveBuffer && PORT_DATA.receivedBytes < ports[port].receiveSize && OrangutanSVPRXBuffer::getReceivedBytes())
		{
			serial_rx(port, OrangutanSVPRXBuffer::getNextByte());
		}

		#else
		#error "Need to fill this in."
		#endif

		// Return because the rest of this function only works for UART-based serial ports.
		return;
	}
	#endif

	// This serial port is a UART.

	// Disable the RX interrupt so it doesn't interrupt this function.
	*ucsrb(port) &= ~(1<<RXCIE);

	if(ports[port].receiveBuffer && ports[port].receivedBytes < ports[port].receiveSize && *ucsra(port) & (1<<RXC)) // A byte has been received
	{
		serial_rx(port, *udr(port));
	}

	// Re-enable the RX interrupt if desired.
	if(ports[port].mode == SERIAL_AUTOMATIC)
	{
		*ucsrb(port) |= (1<<RXCIE);
	}
}

// This is the ISR for receiving bytes and is also called in serial_rx_check.
inline void OrangutanSerial::serial_rx(unsigned char port, unsigned char byte_received)
{
	if(ports[port].receiveBuffer && ports[port].receivedBytes < ports[port].receiveSize)
	{
		ports[port].receiveBuffer[ports[port].receivedBytes] = byte_received;
		ports[port].receivedBytes++; // the byte has been received
	}
	if(ports[port].receivedBytes == ports[port].receiveSize && ports[port].receiveRingOn)
	{
		ports[port].receivedBytes = 0; // reset the ring
	}
}

MULTIPORT_PUBLIC void OrangutanSerial::receive(unsigned char port, char *buffer, unsigned char size)
{
	initModeIfNeeded();

	// TODO: disable the reception interrupt here and re-enable it if necessary before returning

	ports[port].receiveBuffer = buffer;
	ports[port].receivedBytes = 0;
	ports[port].receiveSize = size;
	ports[port].receiveRingOn = 0;
}

MULTIPORT_PUBLIC char OrangutanSerial::receiveBlocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout_ms)
{
	// TODO: Save code-space by calling the non-inlined version of the function? (uncomment lines below)
//#if SERIAL_PORTS > 1
	receive(port, buffer, size);
//#else
//	receive(buffer, size);
//#end

	unsigned long start_time = get_ms();
	while(!receiveBufferFull(port) && (get_ms()-start_time) < timeout_ms)
	{
	    OrangutanSerial::check();
	}

	if(!receiveBufferFull(port))
	{
		return 1;
	}
	return 0;
}

MULTIPORT_PUBLIC void OrangutanSerial::receiveRing(unsigned char port, char *buffer, unsigned char size)
{
	receive(port, buffer, size);
	ports[port].receiveRingOn = 1;
}

MULTIPORT_PUBLIC void OrangutanSerial::cancelReceive(unsigned char port)
{
	receive(port,0,0);
}

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	OrangutanSerial::serial_rx(0, UDR0);
}
#endif

#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	OrangutanSerial::serial_rx(0, UDR0);
}
#endif

#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	OrangutanSerial::serial_rx(1, UDR1);
}
#endif

/** SENDING *******************************************************************/

inline void uart_enable_tx_interrupt(unsigned char port)
{
	*ucsrb(port) |= (1<<UDRIE0);
}

inline void uart_disable_tx_interrupt(unsigned char port)
{
	*ucsrb(port) &= ~(1<<UDRIE0);
}

// Enable the UDRE-empty interrupt if there is data to be sent and we
// are requesting interrupts.  Otherwise, disable it.
inline void OrangutanSerial::uart_update_tx_interrupt(unsigned char port)
{
	if(ports[port].sendBuffer && ports[port].sentBytes < ports[port].sendSize && ports[port].mode == SERIAL_AUTOMATIC)
	{
		uart_enable_tx_interrupt(port);
	}
	else
	{
		uart_disable_tx_interrupt(port);
	}
}

// Inner function to be called by the ISR and by serial_check.
inline void OrangutanSerial::serial_tx_check(unsigned char port)
{
	#ifdef USB
	if (port==USB)
	{
		if(ports[USB].sendBuffer && ports[USB].sentBytes < ports[USB].sendSize)
		{
			#if defined(ORANGUTAN_SVP)
			if (OrangutanSVP::serialSendIfReady(ports[USB].sendBuffer[ports[USB].sentBytes]))
			{
				ports[USB].sentBytes++; // we started sending a byte
			}
			#elif defined(ORANGUTAN_X2)
			#error "Need to fill this in."
			#else
			#error "Need to fill this in."
			#endif
		}
		return;
	}
	#endif

	// This serial port is a UART, not a USB/SPI port.

	// Disable the TX interrupt so it doesn't interrupt this next part.
	uart_disable_tx_interrupt(port);

	uart_tx_isr(port);
}

// TODO: fix the lameness of checking the UDRE bit inside the ISR?  We have to check it because this
// function is also called in the main loop.
inline void OrangutanSerial::uart_tx_isr(unsigned char port)
{
	if(ports[port].sendBuffer && ports[port].sentBytes < ports[port].sendSize && *ucsra(port) & (1<<UDRE))
	{
	    *udr(port) = ports[port].sendBuffer[ports[port].sentBytes];
		ports[port].sentBytes++; // we started sending a byte
	}

	// If called from an interrupt, this will disable the interrupt so we don't get called again.
	// If called from the main loop, this will re-enable the interrupt if appropriate.
	uart_update_tx_interrupt(port);
}

MULTIPORT_PUBLIC void OrangutanSerial::send(unsigned char port, char *buffer, unsigned char size)
{
	initModeIfNeeded();

	ports[port].sendBuffer = buffer;
	ports[port].sentBytes = 0;
	ports[port].sendSize = size;

	// enable the interrupts, and everything will be started by the ISR
	#ifdef USB
	if (port != USB)
	#endif
	{
		uart_update_tx_interrupt(port);
	}
}

MULTIPORT_PUBLIC void OrangutanSerial::sendBlocking(unsigned char port, char *message, unsigned char size)
{
	send(port, message, size);

	// wait for sending before returning
	while(!sendBufferEmpty(port)){ check(); }
}

#ifdef USART_UDRE_vect
ISR(USART_UDRE_vect)
{
	OrangutanSerial::uart_tx_isr(0);
}
#endif

#ifdef USART0_UDRE_vect
ISR(USART0_UDRE_vect)
{
	OrangutanSerial::uart_tx_isr(0);
}
#endif

#ifdef USART1_UDRE_vect
ISR(USART1_UDRE_vect)
{
	OrangutanSerial::uart_tx_isr(1);
}
#endif



// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
