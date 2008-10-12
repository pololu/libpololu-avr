Loading the slave program onto a 3pi causes it to listen for serial
commands set into pin PD0 (RX) and respond with sensor data and status
information on pin PD1 (TX).  This is particularly suited for use with
one of the 3pi expansion kits, which allow you to expand 3pi with
additional electronics.

For an example master program to control this, see the example
3pi-serial-master.

The slave program expects serial data at a rate of 115.2 kbaud, and it
accepts a command set similar to the TReX motor controller.  Each
command is made up of a single command byte in the range 0x80 to 0xff
(128 to 255) and zero or more data bytes, each of which is between
0x00 and 0x7f (0 to 127).

After a complete command is received, the slave processes it and,
depending on the command, responds with zero or more response bytes,
as specified below.

The slave program can process commands as fast as they are sent - as
long as the program sending the commands waits for all response data
to arrive before sending the next command.

The available commands are as follows:

0x81: signature
Data bytes: none
Response: "3pi1.0"

Returns the device signature "3pi1.0".  The version number "1.0" will
be incremented in later versions of this code.

0x86: sensor values
Data bytes: none
Response: 10 bytes (5 integers)

Reads from the line sensors and returns the five values, which range
from 0 (pure white) to 2000 (pure black) sequentially as two-byte
integers.  The AVR is a "little endian" device, which means that
two-byte integers are stored internally with the least significant
byte first, and this protocol follows that convention, sending the
least significant byte first.  For example, a sensor reading of 1100
would be sent as a 76 (0x4c) followed by a 4 (0x04).

0x87: calibrated sensor values
Data bytes: none
Response: 10 bytes (5 integers)

Just like 0x86, but returns calibrated sensor values between 0 to
1000.  See 0xB4, below.

0xB0: trimpot
Data bytes: none
Reponse: 2 bytes (1 integer)

Reads the position of the user trimpot and returns its value as a two
byte integer between 0 and 1023.

0xB1: battery millivolts
Data bytes: none
Response: 2 bytes (1 integer)

Reads the battery voltage and returns it as a integer number of
millivolts.  For examples, at 5.231 V, this will return the number
5231.

0xB3: play
Data bytes: 1-128
Response: none

Plays a tune with the play() function in the Pololu library.  See the
library documentation for the string format.  The first byte that is
sent specifies the length of the following string, which is passed to
the play() function and may be up to 127 bytes long.  For example,
sending the bytes

  0xB3, 3, 'c', 'e', 'g'

will play the notes of a C major triad.

0xB4: calibrate line sensors
Data bytes: none
Response: 10 bytes (5 integers)

This function runs calibrate() from the Pololu library, taking 10
sensor readings and recording the minimum and maximum values.  It then
takes an additional reading and sends the calibrated results, as with
command 0xB7.

0xB5: reset calibration
Data bytes: none
Response: none

Resets the calibration using the line_sensors_reset_calibration()
function from the Pololu library.  This is important to do at the
beginning of any code that uses calibration, since if the master is
reset without resetting the slave (for example due to a power glitch),
old calibration values might remain in the slave's memory.

0xB6: line position
Data bytes: none
Response: 2 bytes (1 integer)

Using the read_line() function from the Pololu library, reads
calibrated values from the line sensors and returns a single integer
representing the position of a black line under the robot.  A value of
0 means that the line is all the way to the left side of the 3pi, and
a value of 4000 means that it is all the way to the right side.  See
the library documentation for details on the algorithm used to compute
the line position.

0xC1: motor 1 forward
Data bytes: 1 byte (0-127)
Response: none

Sets motor 1 forward to a speed from 0 (off) to 127 (maximum speed).

0xC2: motor 1 backward
Data bytes: 1 byte (0-127)
Response: none

Sets motor 1 backward to a speed from 0 (off) to 127 (maximum speed).

0xC5: motor 2 forward
Data bytes: 1 byte (0-127)
Response: none

Sets motor 2 forward to a speed from 0 (off) to 127 (maximum speed).

0xC6: motor 2 backward
Data bytes: 1 byte (0-127)
Response: none

Sets motor 2 backward to a speed from 0 (off) to 127 (maximum speed).
