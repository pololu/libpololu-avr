/*
  OrangutanSerial.cpp - Library for serial transmit and receive.
*/

/*
 * Written by Paul Grayson et al., October, 2008.
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
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
#include "../OrangutanX2/OrangutanX2.h"
#include "../OrangutanResources/include/OrangutanModel.h"

#include <avr/io.h>
#include <avr/interrupt.h>


extern "C" void serial_check()
{
	OrangutanSerial::check();
}

#if _SERIAL_PORTS > 1

/** MULTI-PORT C FUNCTIONS ****************************************************/
// C functions for device with multiple serial ports.  A port argument is
// necessary.

extern "C" void serial_set_mode(unsigned char port, unsigned char mode)
{
	OrangutanSerial::setMode(port, mode);
}

extern "C" unsigned char serial_get_mode(unsigned char port)
{
	return OrangutanSerial::getMode(port);
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

/** SINGLE-PORT C FUNCTIONS ***************************************************/
// C Functions for devices with one UART.  No port argument is necessary.
// These functions simply call the C++ single port functions, which are defined
// below.

extern "C" void serial_set_mode(unsigned char mode)
{
	OrangutanSerial::setMode(mode);
}

extern "C" unsigned char serial_get_mode()
{
	return OrangutanSerial::getMode();
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


#if _SERIAL_PORTS==1

/** SINGLE-PORT C++ FUNCTIONS *************************************************/
// C++ Functions for devices with one UART.  No port argument is necessary.
// These functions call the multi-port C++ functions which are inline because
// they are declared with _SINGLE_PORT_INLINE, so the compiler will know ahead of
// time that the port argument to those is going to be zero and it can make a lot
// of optimizations in these functions.

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

SerialPortData OrangutanSerial::ports[_SERIAL_PORTS] =
{
	{mode:SERIAL_AUTOMATIC, sentBytes:0, receivedBytes:0, sendSize:0, receiveSize:0, receiveRingOn:0, sendBuffer:0, receiveBuffer:0},
#if _SERIAL_PORTS > 1
	{mode:SERIAL_AUTOMATIC, sentBytes:0, receivedBytes:0, sendSize:0, receiveSize:0, receiveRingOn:0, sendBuffer:0, receiveBuffer:0},
	{mode:SERIAL_CHECK,     sentBytes:0, receivedBytes:0, sendSize:0, receiveSize:0, receiveRingOn:0, sendBuffer:0, receiveBuffer:0},
#endif
};

/** PRIVATE PROTOTYPES ********************************************************/
inline void uart_update_tx_interrupt(unsigned char port);
inline void serial_tx_check(unsigned char port);
inline void serial_rx_check(unsigned char port);

/** HELPERS *******************************************************************/

// UART-generic bit numbers.
#define RXEN  RXEN0
#define TXEN  TXEN0
#define RXCIE RXCIE0
#define RXEN  RXEN0
#define TXEN  TXEN0
#define UDRE  UDRE0
#define RXC   RXC0

#if _SERIAL_PORTS > 1

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

// empty constructor
OrangutanSerial::OrangutanSerial()
{

}

void OrangutanSerial::initUART_inline(unsigned char port)
{
	if (!_PORT_IS_UART)
	{
		return;
	}

	// Enable receiver and transmitter.
	// Enable reception interrupt if desired.
	if(ports[port].mode == SERIAL_AUTOMATIC)
	{
		*ucsrb(port) = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);
	}
	else
	{
		*ucsrb(port) = (1<<RXEN) | (1<<TXEN);
	}

	// Enable transmission interrupt if desired.
	uart_update_tx_interrupt(port);

	// Enable interrupts in general.
	sei();
}

void OrangutanSerial::initPort(unsigned char port)
{
	// We use a case statement instead of just "initUART_inline(port)"
	// because that allows us to save about 100 bytes of program space.  That's
	// because this method allows the compiler to compute ahead of time the
	// exact addresses of all the variables accessed by initUART_inline, so it
	// does not need to deal with pointers.

	#if _SERIAL_PORTS > 1
	switch(port)
	{
		case 0:	initUART_inline(0); break; // Initialize UART0
		case 1: initUART_inline(1); break; // Initialize UART1
		// No initialization is required for the USB-based serial port.
	}
	#else
	initUART_inline(0);
	#endif
}

_SINGLE_PORT_INLINE void OrangutanSerial::setBaudRate(unsigned char port, unsigned long baud)
{
	initPort(port);

	unsigned int baud_ubrr = (F_CPU - 8*baud) / (16*baud);

	if (!_PORT_IS_UART)
	{
		// You can't set the baud rate on the virtual COM port on the Orangutan SVP,
		// but you can on the Orangutan X2 since it uses a CP2102 USB-to-UART bridge:
		#ifdef _ORANGUTAN_X2
		OrangutanX2::setSerial(UART_NO_PARITY, UART_ONE_STOP_BIT, UART_NORMAL_SPEED, baud_ubrr, 0);
		#endif
		return;
	}

	*ubrr(port) = baud_ubrr;
}

_SINGLE_PORT_INLINE void OrangutanSerial::setMode(unsigned char port, unsigned char mode)
{
	ports[port].mode = mode;

	// Disable/Enable the UART RX/TX interrupts as required.
	initPort(port);
}

void OrangutanSerial::check()
{
	serial_tx_check(0);

	#if _SERIAL_PORTS > 1
	serial_tx_check(1);
	#endif

	#if _SERIAL_PORTS > 2
	serial_tx_check(2);
	#endif

	serial_rx_check(0);

	#if _SERIAL_PORTS > 1
	serial_rx_check(1);
	#endif

	#if _SERIAL_PORTS > 2
	serial_rx_check(2);
	#endif
}

/** RECEIVING *****************************************************************/

#ifdef _ORANGUTAN_SVP
// The low-level SPI protocol for the Orangutan SVP's auxiliary processor only
// lets the AVR receive bytes in chunks of unknown size.  When the AVR asks to
// receive data, the auxiliary processor tells the AVR how many bytes are in the
// buffer, and then the AVR must read all of them or else they will be lost.
// The auxiliary buffer will send at most 8 bytes at a time.  Therefore, we
// implement this 8-byte buffer so the user can receive bytes at whatever pace
// he wants.
namespace OrangutanSVPRXFIFO
{
	// The number of bytes received that are in the buffer.
	static unsigned char byte_count;

	// Holds the bytes received.  The bytes are stored backwards, starting
	// at buffer[byte_count] for the first byte down to buffer[0] for the last.
	// This lets us implement getNextByte without having to shift the whole
	// buffer by one.
	static unsigned char buffer[8];

	// Returns the number of bytes in the buffer to be received.
	static unsigned char getReceivedBytes()
	{
		if (byte_count == 0)
		{
			// The buffer is empty, so we can ask the auxiliary processor
			// for more data.
			byte_count = OrangutanSVP::serialReadStart();
			for (unsigned char i=0; i < byte_count; i++)
			{
				buffer[byte_count-1-i] = OrangutanSVP::getNextByte();
			}
		}
		return byte_count;
	}

	// Removes one byte from the buffer and returns it.
	static unsigned char getNextByte()
	{
		byte_count--;
		return buffer[byte_count];
	}
}
#endif

// This is called in check() to take care of receiving bytes.
// It is only called with a constant port argument, so we don't need
// to worry about overhead form functions like ucsrb(port), or
// accessing variables in ports[port].  The compiler knows ahead of time
// the exact locations of those variables.
inline void OrangutanSerial::serial_rx_check(unsigned char port)
{
	if (_PORT_IS_UART)
	{
		// This serial port is a UART.

		// Disable the RX interrupt so it doesn't interrupt this function.
		*ucsrb(port) &= ~(1<<RXCIE);

		if(ports[port].receiveBuffer && ports[port].receivedBytes < ports[port].receiveSize && *ucsra(port) & (1<<RXC)) // A byte has been received
		{
			serial_rx_handle_byte(port, *udr(port));
		}

		// Re-enable the RX interrupt if desired.
		if(ports[port].mode == SERIAL_AUTOMATIC)
		{
			*ucsrb(port) |= (1<<RXCIE);
		}
	}
    #ifdef USB_COMM
	else if (port==USB_COMM)
	{
		// While we are trying to receive bytes, and a byte has been received...
		while(ports[USB_COMM].receiveBuffer && ports[USB_COMM].receivedBytes < ports[USB_COMM].receiveSize && BYTES_RECEIVED)
		{
			// We don't call serial_rx_handle_byte here, because that function resets receivedBytes
			// during ring reception mode, which could cause an infinite loop here.

			ports[USB_COMM].receiveBuffer[ports[USB_COMM].receivedBytes] = NEXT_BYTE;
			ports[USB_COMM].receivedBytes++; // the byte has been received

			if(ports[USB_COMM].receivedBytes == ports[USB_COMM].receiveSize && ports[USB_COMM].receiveRingOn)
			{
				ports[USB_COMM].receivedBytes = 0; // reset the ring

				// Return here so we don't have an infinite loop.
				return;
			}
		}
	}
	#endif
}

// This function is called whenever a byte has been received on the serial port
// and it needs to be handled.  It is called from serial_rx_check and also from
// the ISR (interrupt service routine).  In both cases, it is called with a 
// constant port argument (or from an inline function with a constant port
// argument) so we needn't worry about overhead from expressions like ports[port].
inline void OrangutanSerial::serial_rx_handle_byte(unsigned char port, unsigned char byte_received)
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

inline void OrangutanSerial::receive_inline(unsigned char port, char * buffer, unsigned char size, unsigned char receiveRingOn)
{
	// Disable the RX interrupt if necessary.
	if (_PORT_IS_UART)
	{
		*ucsrb(port) &= ~(1<<RXCIE);
	}

	ports[port].receiveBuffer = buffer;
	ports[port].receivedBytes = 0;
	ports[port].receiveSize = size;
	ports[port].receiveRingOn = receiveRingOn;

	if (_PORT_IS_UART && ports[port].mode == SERIAL_AUTOMATIC)
	{
		// Re-enable the RX interrupt so background receiving will work.
		*ucsrb(port) |= (1<<RXCIE);
	}
}

_SINGLE_PORT_INLINE void OrangutanSerial::receive(unsigned char port, char *buffer, unsigned char size)
{
	receive_inline(port, buffer, size, 0);
}

_SINGLE_PORT_INLINE char OrangutanSerial::receiveBlocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout_ms)
{
	receive(port, buffer, size);

	unsigned long start_time = get_ms();

	while(1)
	{
	    check();

		if (receiveBufferFull(port))
		{
			return 0; // Success
		}

		if (get_ms()-start_time >= timeout_ms)
		{
			return 1; // Timeout
		}
	}
}

_SINGLE_PORT_INLINE void OrangutanSerial::receiveRing(unsigned char port, char *buffer, unsigned char size)
{
	receive_inline(port, buffer, size, 1);
}

_SINGLE_PORT_INLINE void OrangutanSerial::cancelReceive(unsigned char port)
{
	receive(port,0,0);
}

#ifdef USART_RX_vect
ISR(USART_RX_vect)
{
	OrangutanSerial::serial_rx_handle_byte(0, UDR0);
}
#endif

#ifdef USART0_RX_vect
ISR(USART0_RX_vect)
{
	OrangutanSerial::serial_rx_handle_byte(0, UDR0);
}
#endif

#ifdef USART1_RX_vect
ISR(USART1_RX_vect)
{
	OrangutanSerial::serial_rx_handle_byte(1, UDR1);
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
	if (_PORT_IS_UART)
	{
		// This serial port is a UART, not a USB/SPI port.

		// Disable the TX interrupt so it doesn't interrupt this next part.
		uart_disable_tx_interrupt(port);

		uart_tx_isr(port);
	}
	#ifdef USB_COMM
	else if (port==USB_COMM)
	{
		while(1)
		{
			if(!ports[USB_COMM].sendBuffer || ports[USB_COMM].sentBytes >= ports[USB_COMM].sendSize)
			{
				// Return because we have nothing (more) to send.
				return;
			}
			
			if (SEND_BYTE_IF_READY(ports[USB_COMM].sendBuffer[ports[USB_COMM].sentBytes]))
			{
				// We successfully started sending a byte
				ports[USB_COMM].sentBytes++;

				// Try to send another byte.
				continue;
			}
			
			// Return because we can not send any more bytes.
			return;
		}
	}
	#endif
}

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

_SINGLE_PORT_INLINE void OrangutanSerial::send(unsigned char port, char *buffer, unsigned char size)
{
	ports[port].sendBuffer = buffer;
	ports[port].sentBytes = 0;
	ports[port].sendSize = size;

	// enable the interrupts, and everything will be started by the ISR
	if (_PORT_IS_UART)
	{
		uart_update_tx_interrupt(port);
	}
}

_SINGLE_PORT_INLINE void OrangutanSerial::sendBlocking(unsigned char port, char *buffer, unsigned char size)
{
	send(port, buffer, size);

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
