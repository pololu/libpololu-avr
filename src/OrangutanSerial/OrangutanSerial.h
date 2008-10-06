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

class OrangutanSerial
{
public:

    // Constructor (doesn't do anything).
	OrangutanSerial();
	
	// Sets the serial library to a given baudrate.
	static void setBaudRate(unsigned long baud);

	// Sets up a buffer for background receive.
	// Data will go into this buffer until size bytes have been
	// stored.
	static void receive(char *buffer, unsigned char size);

	// Gets the number of bytes that have been received since
	// receive() was called.
	static inline unsigned char getReceivedBytes() { return receivedBytes; }

	// True when the receive buffer is full.
	static inline char receiveBufferFull() { return getReceivedBytes() == receiveSize; }

	// Sets up a buffer for background transmit.
	// Data from this buffer will be transmitted until size bytes have
	// been sent.
	static void send(char *buffer, unsigned char size);

	// Gets the number of bytes that have been sent since send() was
	// called.
	static inline unsigned char getSentBytes() { return sentBytes; }

	// True when the receive buffer is empty.
	static inline char sendBufferEmpty() { return getSentBytes() == sendSize; }

	static unsigned char sentBytes;
	static unsigned char receivedBytes;
	static unsigned char sendSize;
	static unsigned char receiveSize;

	static char *sendBuffer;
	static char *receiveBuffer;

private:
	static void init();
};


// Local Variables: **
// mode: C++ **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
