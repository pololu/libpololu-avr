Pololu AVR Library - Source distribution

Copyright (c) 2008 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J18
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
* Pololu Orangutan LV-168

Additionally, this library supports:

* Pololu QTR-1A and QTR-8A reflectance sensors for use with analog inputs
* Pololu QTR-1RC and QTR-8RC reflectance sensors for use with digital inputs
* The Pololu 3pi Robot, a complete robot kit to be released in August 2008

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

  http://www.atmel.com/dyn/Products/tools_card.asp?tool_id=2725

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

  C:\WinAVR-20070525\avr

In Linux, the avr-gcc files are probably located in

  /usr/avr

(Note: if you are using Linux and the avr-gcc files are in /usr/avr,
you can install the library simple by unpacking this archive and
typing "sudo make install" from within the directory that is created.)

If you currently have an older version of the Pololu AVR Library, your
first step should be to delete all of the old include files and the
libpololu.a file that you installed previously.

Next, copy libpololu.a into the "lib" subdirectory.

Finally, copy the entire "pololu" subfolder into the "include"
subdirectory.

You are now ready to use the Pololu AVR library.


== Using the Pololu AVR Library ==

A detailed command reference is provided at

 http://www.pololu.com/docs/0J18

To use the library with C, you must place one of the following lines

#include <pololu/orangutan.h>
#include <pololu/3pi.h>
#include <pololu/qtr.h>

at the top of any C file that uses functions provided by the library.
To use the library with C++, the equivalent directives are

#include <pololu/orangutan>
#include <pololu/Pololu3pi.h>
#include <pololu/PololuQTRSensors.h>

The line or lines that you include depend on which product you are
using with the library.

When compiling, you must link your object files with libpololu.a.
This is accomplished by passing the -lpololu option to avr-gcc during
the linking step.

To add the -lpololu option within AVR studio, select

 Project -> Configuration Options -> Libraries

You should see libpololu.a listed as an option on the left column.
Select this file and click "add library" to add it to your project.


== Example AVR Studio Project ==

A very simple demo program for the Orangutan is available in the
folder

  test\avr-studio-project

Double-click on the file test.aps, and the project should open
automatically in AVR Studio, showing a C file that uses a few basic
commands from the Pololu AVR Library.  To compile this program, select
Build -> Build or press F7.  Look for warnings and errors (indicated
by yellow and red dots) in the output displayed below.  If the program
compiles successfully, the message

  Build succeeded with 0 Warnings...

will appear at the end of the output, and a file "test.hex" will have
been created in the test\avr-studio-project\default folder.

Connect your Orangutan Programmer to your computer and your
Orangutan board, and turn on the Orangutan's power.  The green status
LED close to the USB connector should be on, while the other two LEDs
should be off, indicating that the programmer is ready.  

Select

  Tools -> Program AVR -> Connect

to connect to the Orangutan Programmer.  The default options of
"STK500 or AVRISP" and "Auto" should be fine, so click Connect and the
AVRISP programming window should appear.

You will use AVRISP to load "test.hex" into the flash memory of your
AVR.  To do this, click "Browse..." in the Flash section and select
file "test.hex" that was compiled earlier.  Note that you have to
first navigate to your project directory!  Now click "Program" in the
Flash section, and the test code should be loaded onto your Orangutan.

If your Orangutan was successfully programmed, you should hear a short
tune, see the message "Hello!" on the LCD (if one is present), and the
LEDs on the board should blink.
