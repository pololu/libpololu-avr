Pololu AVR Library

Copyright (c) 2008-2010 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J20
http://www.pololu.com
http://forum.pololu.com

You may freely modify and share this code, as long as you keep this
notice intact (including the three links above).  Licensed under the
Creative Commons BY-SA 3.0 license:

http://creativecommons.org/licenses/by-sa/3.0/

Disclaimer: To the extent permitted by law, Pololu provides this work
without any warranty.  It might be defective, in which case you agree
to be responsible for all resulting costs and damages.


== Features ===

This library includes support for recent members of the Orangutan line
of robot controllers:

* Pololu Baby Orangutan B-48
* Pololu Baby Orangutan B-168
* Pololu Baby Orangutan B-328
* Pololu Orangutan LV-168
* Pololu Orangutan SV-168
* Pololu Orangutan SV-328
* Pololu Orangutan SVP-324
* Pololu Orangutan SVP-1284

Additionally, this library supports:

* Pololu QTR-1A and QTR-8A reflectance sensors for use with analog inputs
* Pololu QTR-1RC and QTR-8RC reflectance sensors for use with digital inputs
* Pololu Wheel Encoders
* The Pololu 3pi Robot, a complete robot kit based on the Orangutan line


== Prerequisites ==

The free avr-gcc compiler, avr-libc, and other associated tools must
be installed before the Pololu AVR library.  For Windows users, these
tools are made available as the WinAVR distribution:

  http://winavr.sourceforge.net/

Linux users should install all AVR-related packages available in their
distribution's package manager.  In particular, under Ubuntu you will
want to install the following packages:

  avr-libc
  gcc-avr
  avra
  binutils-avr
  avrdude (for use with the Pololu Orangutan Programmer)

For Windows users, we also recommend the AVR Studio development
environment, which may be downloaded free of charge from Atmel at

  http://www.atmel.com/avrstudio

To program the Pololu Orangutan or 3pi, we recommend the Pololu
Orangutan Programmer.  If you will be using a Pololu programmer,
follow the instructions at

  http://www.pololu.com/docs/0J6

to install it on your computer.


== Extracting the archive ==

Windows: open the .zip file and click "Extract all" to extract the
Pololu AVR Library files.

Linux: run the command "unzip libpololu-avr-xxx.zip", where xxx is
replaced by the version of the library that you have downloaded.

A directory called "libpololu-avr" will be created.


== Compiling the Pololu AVR Library (Source distribution only) ==

Unpack the entire archive and open a command prompt within the
libpololu-avr directory.  If avr-gcc is correctly installed on your
system, you will be able to type "make" to compile the entire
library.  Pay attention to any errors that occur during the build
process.


== Installation of the Pololu AVR Library ==

Determine the location of your avr-gcc files.  In Windows, they will
usually be in a folder such as:

  C:\WinAVR-20090313\avr

In Linux, the avr-gcc files are probably located in

  /usr/avr

(Note: if you are using Linux and the avr-gcc files are in /usr/avr,
you can install the library simple by unpacking this archive and
typing "sudo make install" from within the directory that is created.)

If you currently have an older version of the Pololu AVR Library, your
first step should be to delete all of the old include files and the
libpololu*.a files that you installed previously.

Next, copy libpololu_*.a into the "lib" subdirectory.

Finally, copy the entire "pololu" subfolder into the "include"
subdirectory.

You are now ready to use the Pololu AVR library.


== Using the Pololu AVR Library ==

For detailed instructions on using the library, please see our
instructions at:

  http://www.pololu.com/docs/0J20

