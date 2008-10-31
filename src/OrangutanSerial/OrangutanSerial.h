/*
  OrangutanSerial.h - Library for serial transmit and receive.
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

#define SERIAL_AUTOMATIC 0
#define SERIAL_CHECK 1

class OrangutanSerial
{
public:

	// Constructor (doesn't do anything).
	OrangutanSerial();
	
	// Sets the serial library to a given baudrate.
	static void setBaudRate(unsigned long baud);

	// Sets the serial library to use either a polling scheme
	// (SERIAL_CHECK) or interrupts (SERIAL_AUTOMATIC; the default)
	// for sending and receiving serial data.  If the mode is set to
	// SERIAL_CHECK, the function serialCheck() must be called
	// periodically to trigger reception and transmission of new
	// bytes.
	static void setMode(unsigned char mode);

	// Returns the current serial mode
	static inline unsigned char getMode() { return mode; }

	// This function should be called periodically when in
	// SERIAL_CHECK mode.
	static void check();

	// Sets up a buffer for background receive.
	// Data will go into this buffer until size bytes have been
	// stored.
	static void receive(char *buffer, unsigned char size);

	// Same as receive(), but waits until the buffer is full or the
	// timeout has elapsed to return.  Returns true on timeout, false
	// on buffer fill.
	static char receiveBlocking(char *buffer, unsigned char size, unsigned int timeout_ms);

	// Sets up a ring buffer for background receive.
	// Data will go into this buffer, and when size bytes have been
	// stored, it will wrap around to the beginning, with
	// getReceivedBytes reset to 0.
	static void receiveRing(char *buffer, unsigned char size);

	// Stops receiving serial bytes.
	static void cancelReceive();

	// Gets the number of bytes that have been received since
	// receive() was called.
	static inline unsigned char getReceivedBytes() { return receivedBytes; }

	// True when the receive buffer is full.
	static inline char receiveBufferFull() { return getReceivedBytes() == receiveSize; }

	// Sets up a buffer for background transmit.
	// Data from this buffer will be transmitted until size bytes have
	// been sent.  If send() is called before sendBufferEmpty()
	// returns true (when transmission of the last byte has started),
	// the old buffer will be discarded and tramission will be cut
	// short.  This means that you should almost always wait until the
	// data has been sent before calling this function again.  See
	// sendBlocking(), below, for an easy way to do this.
	static void send(char *buffer, unsigned char size);

	// Same as send(), but waits until transmission of the last byte
	// has started to return.  When this function returns, it is safe
	// to call send() or sendBlocking() again.
	static void sendBlocking(char *buffer, unsigned char size);

	// Gets the number of bytes that have been sent since send() was
	// called.
	static inline unsigned char getSentBytes() { return sentBytes; }

	// True when the receive buffer is empty.
	static char sendBufferEmpty() { return sentBytes == sendSize; }

	volatile static unsigned char sentBytes;
	volatile static unsigned char receivedBytes;
	static unsigned char sendSize;
	static unsigned char receiveSize;
	static unsigned char receiveRingOn; // boolean

	static char *sendBuffer;
	static char *receiveBuffer;

private:
	static void init();
	static unsigned char mode;
};


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
