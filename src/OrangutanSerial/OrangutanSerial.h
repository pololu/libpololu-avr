/*
  OrangutanSerial.h - Library for serial transmit and receive.
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

#ifndef OrangutanSerial_h
#define OrangutanSerial_h

#include "../OrangutanResources/include/OrangutanModel.h"

#include <avr/interrupt.h>

#if defined(_ORANGUTAN_SVP)
 // The Orangutan SVP has two UARTs and one virtual COM port via USB.
 #define _SERIAL_PORTS 				3
 #define UART0    					0
 #define UART1    					1
 #define USB_COMM 					2
 #define _PORT_IS_UART 				(port!=2)
 #define BYTES_RECEIVED				OrangutanSVPRXFIFO::getReceivedBytes()
 #define NEXT_BYTE					OrangutanSVPRXFIFO::getNextByte()
 #define SEND_BYTE_IF_READY(byte)	OrangutanSVP::serialSendIfReady(byte)
#elif defined(_ORANGUTAN_X2)
 // The Orangutan X2 has two UARTs and one virtual COM port via USB.
 #define _SERIAL_PORTS 				3
 #define UART0 						0
 #define UART1 						1
 #define USB_COMM   				2
 #define _PORT_IS_UART 				(port!=2)
 #define BYTES_RECEIVED				OrangutanX2::getNumRXBytes()
 #define NEXT_BYTE					OrangutanX2::readSerialByte()
 #define SEND_BYTE_IF_READY(byte)	OrangutanX2::sendSerialByteIfReady(byte)
#else
 #define _SERIAL_PORTS 1
 #define _PORT_IS_UART 1
#endif

#if _SERIAL_PORTS > 1
    #define _SINGLE_PORT_INLINE
#else
    #define _SINGLE_PORT_INLINE inline
#endif

#define SERIAL_AUTOMATIC 0
#define SERIAL_CHECK 1

#ifdef __cplusplus

typedef struct SerialPortData
{
	unsigned char mode;	// SERIAL_AUTOMATIC (interrupt-driven) or SERIAL_CHECK
	volatile unsigned char sentBytes;
	volatile unsigned char receivedBytes;
	unsigned char sendSize;
	unsigned char receiveSize;
	unsigned char receiveRingOn; // boolean
	char *sendBuffer;
	char *receiveBuffer;
} SerialPortData;

class OrangutanSerial
{
  public:

	// Constructor (doesn't do anything).
	OrangutanSerial();

	// check: This function should be called periodically when in
	// SERIAL_CHECK mode.
	static void check();

	// setBaudRate: Sets the serial port to a given baudrate.

	// setMode: Sets the serial library to use either a polling scheme
	// (SERIAL_CHECK) or interrupts (SERIAL_AUTOMATIC; the default)
	// for sending and receiving serial data.  If the mode is set to
	// SERIAL_CHECK, the function serialCheck() must be called
	// periodically to trigger reception and transmission of new
	// bytes.

	// getMode: Returns the current serial mode

	// receive: Sets up a buffer for background receive.
	// Data will go into this buffer until size bytes have been
	// stored.

	// receiveBlocking: Same as receive(), but waits until the buffer is full or the
	// timeout has elapsed to return.  Returns true on timeout, false
	// on buffer fill.

	// receiveRing: Sets up a ring buffer for background receive.
	// Data will go into this buffer, and when size bytes have been
	// stored, it will wrap around to the beginning, with
	// getReceivedBytes reset to 0.

	// cancelReceive: Stops receiving serial bytes.

	// getReceivedBytes: Gets the number of bytes that have been received since
	// receive() was called.

	// receiveBufferFull: True when the receive buffer is full.

	// send: Sets up a buffer for background transmit.
	// Data from this buffer will be transmitted until size bytes have
	// been sent.  If send() is called before sendBufferEmpty()
	// returns true (when transmission of the last byte has started),
	// the old buffer will be discarded and tramission will be cut
	// short.  This means that you should almost always wait until the
	// data has been sent before calling this function again.  See
	// sendBlocking(), below, for an easy way to do this.

	// sendBlocking: Same as send(), but waits until transmission of the last byte
	// has started to return.  When this function returns, it is safe
	// to call send() or sendBlocking() again.

	// getSentBytes: Gets the number of bytes that have been sent since send() was
	// called.

	// sendBufferEmpty: True when the send buffer is empty.

#if _SERIAL_PORTS == 1
	static void setBaudRate(unsigned long baud);
	static void setMode(unsigned char mode);
	static void receive(char *buffer, unsigned char size);
	static char receiveBlocking(char *buffer, unsigned char size, unsigned int timeout_ms);
	static void receiveRing(char *buffer, unsigned char size);
	static void cancelReceive();
	static void send(char *buffer, unsigned char size);
	static void sendBlocking(char *buffer, unsigned char size);
	static inline char sendBufferEmpty() { return ports[0].sentBytes == ports[0].sendSize; }
	static inline unsigned char getSentBytes() { return ports[0].sentBytes; }
	static inline unsigned char getReceivedBytes() { return ports[0].receivedBytes; }
	static inline char receiveBufferFull() { return getReceivedBytes() == ports[0].receiveSize; }
	static inline unsigned char getMode() { return ports[0].mode; }
#endif

#if _SERIAL_PORTS > 1
  public:
#else
  private:
#endif
	static _SINGLE_PORT_INLINE void setBaudRate(unsigned char port, unsigned long baud);
	static _SINGLE_PORT_INLINE void setMode(unsigned char port, unsigned char mode);
	static _SINGLE_PORT_INLINE void receive(unsigned char port, char *buffer, unsigned char size);
	static _SINGLE_PORT_INLINE char receiveBlocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout_ms);
	static _SINGLE_PORT_INLINE void receiveRing(unsigned char port, char *buffer, unsigned char size);
	static _SINGLE_PORT_INLINE void cancelReceive(unsigned char port);
	static _SINGLE_PORT_INLINE void send(unsigned char port, char *buffer, unsigned char size);
	static _SINGLE_PORT_INLINE void sendBlocking(unsigned char port, char *buffer, unsigned char size);
	static inline char sendBufferEmpty(unsigned char port) { return ports[port].sentBytes == ports[port].sendSize; }
	static inline unsigned char getMode(unsigned char port) { return ports[port].mode; }
	static inline unsigned char getReceivedBytes(unsigned char port) { return ports[port].receivedBytes; }
	static inline char receiveBufferFull(unsigned char port) { return getReceivedBytes(port) == ports[port].receiveSize; }
	static inline unsigned char getSentBytes(unsigned char port) { return ports[port].sentBytes; }

  private:

	static SerialPortData ports[_SERIAL_PORTS];

	static inline void initUART_inline(unsigned char port);
	static inline void receive_inline(unsigned char port, char *buffer, unsigned char size, unsigned char ring);

	static inline void uart_update_tx_interrupt(unsigned char port);
	static inline void serial_tx_check(unsigned char port);
	static inline void serial_rx_check(unsigned char port);

	static void initPort(unsigned char port);

	// Don't call these functions.  They should only be called from the interrupt-service routine
	// defined in OrangutanSerial.cpp.  They only reason they are public is because they need to
	// access private data (ports) and David could not figure out how to make the ISR be inside the class.
  public:
	static inline void uart_tx_isr(unsigned char port);
	static inline void serial_rx_handle_byte(unsigned char port, unsigned char byte_received);
};

extern "C" {
#endif //__cplusplus

// C Function declarations.
void serial_check(void);

#if _SERIAL_PORTS > 1
void serial_set_baud_rate(unsigned char port, unsigned long baud);
void serial_set_mode(unsigned char port, unsigned char mode);
unsigned char serial_get_mode(unsigned char port);
void serial_receive(unsigned char port, char *buffer, unsigned char size);
void serial_cancel_receive(unsigned char port);
char serial_receive_blocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout);
void serial_receive_ring(unsigned char port, char *buffer, unsigned char size);
unsigned char serial_get_received_bytes(unsigned char port);
char serial_receive_buffer_full(unsigned char port);
void serial_send(unsigned char port, char *buffer, unsigned char size);
void serial_send_blocking(unsigned char port, char *buffer, unsigned char size);
unsigned char serial_get_sent_bytes(unsigned char port);
char serial_send_buffer_empty(unsigned char port);
#else
void serial_set_baud_rate(unsigned long baud);
void serial_set_mode(unsigned char mode);
unsigned char serial_get_mode(void);
void serial_receive(char *buffer, unsigned char size);
void serial_cancel_receive(void);
char serial_receive_blocking(char *buffer, unsigned char size, unsigned int timeout);
void serial_receive_ring(char *buffer, unsigned char size);
unsigned char serial_get_received_bytes(void);
char serial_receive_buffer_full(void);
void serial_send(char *buffer, unsigned char size);
void serial_send_blocking(char *buffer, unsigned char size);
unsigned char serial_get_sent_bytes(void);
char serial_send_buffer_empty(void);
#endif

#ifdef __cplusplus
}
#endif

//OrangutanSerial_h
#endif

// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
