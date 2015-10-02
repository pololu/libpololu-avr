Pololu AVR Library

Copyright (c) 2008-2015 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J20
http://www.pololu.com
http://forum.pololu.com


== Features ==

The Pololu AVR Library is a collection of support functions for
programming AVR-based Pololu products or for using Pololu products
with AVRs.  It is designed for use with the free avr-gcc compiler,
which is included with Atmel Studio 6 and WinAVR.  Most of the
library can also be used together with the Arduino environment.

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
* Pololu Orangutan X2

Additionally, this library supports:

* Pololu QTR-1A and QTR-8A reflectance sensors for use with analog inputs
* Pololu QTR-1RC and QTR-8RC reflectance sensors for use with digital inputs
* Pololu Wheel Encoders
* The Pololu 3pi Robot, a complete robot kit based on the Orangutan line


== Installation instructions ==

Detailed installation instructions for Windows, Linux, and Mac OS X
and tutorials for using Atmel Studio can be found in the Pololu AVR
Programming Quick Start Guide:

  http://www.pololu.com/docs/0J51

The general instructions below apply to all development environments.


== Prerequisites ==

The free avr-gcc compiler, avr-libc, and other associated tools must
be installed before the Pololu AVR library.  Once again, please see
the Pololu AVR Programming Quick Start Guide for detailed instructions
for Windows, Linux, and Mac OS X.

To program your AVR, we recommend the Pololu USB AVR Programmer:

  http://www.pololu.com/catalog/product/1300


== Distributions of the Pololu AVR Library ==

The source code of the Pololu AVR Library is available as a git
repository hosted on github:

  http://github.com/pololu/libpololu-avr

The source repository is used to generate a binary distribution that
has compiled binaries (.a files), headers (.h files), examples for
every supported device, Atmel Studio 5 templates, and more.  The
binary distribution contains the source code of the libraries, but it
does NOT include every file from the source repository.  The binary
distributions can be downloaded as zip files.

You can tell what type of distribution you have by looking at what
files are present in it.  The source repository will have a src.mk
file, while the binary distribution will not.

For the convnience of Windows users, the binary distribution is
packaged in an executable installer called the Pololu AVR Library
Installer for Windows.  This installer can be downloaded by itself and
is also included in the Pololu AVR Development Bundle.

The binary distribution zip files, the Pololu AVR Library Installer
for Windows, and the Pololu AVR Development bundle are all available
from the "Download Instructions" section of the Pololu AVR C/C++
Library User's Guide:

  http://www.pololu.com/docs/0J20


== Compiling the Pololu AVR Library ==

Open a command prompt within the library's top-level folder.  If
avr-gcc is correctly installed on your system, you will be able to
type "make" to compile the entire library.  Pay attention to any
errors that occur during the build process.


== Installation using "make install" ==

If you are installing the official version of the Pololu AVR Library
on Windows, we recommend downloading the Pololu AVR Library Installer
for Windows or the Pololu AVR Development Bundle which includes it.

Otherwise, you should determine the location of your avr-gcc files.
This will typically be one of:
* C:\Program Files (x86)\Atmel\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr
* C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\avr
* C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.0.65\AVRToolchain\avr
* C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\avr
* C:\Program Files (x86)\Atmel\AVR Studio 5.0\AVR Toolchain\avr
* C:\WinAVR-20100110\avr
* /usr/lib/avr
* /usr/avr
* /usr/local/CrossPack-AVR/avr/

In this folder you will find an "include" folder that has AVR header
files such as avr/io.h and a "lib" folder that has AVR libraries such
as libc.a.

In the library's top-level folder, type "make show_prefix".  The
Makefile will tell you where it thinks your avr-gcc files are.  If the
folder names displayed are correct, then you can run "make install" to
install the library files.  You will need to run this command with the
correct permissions, so "sudo make install" will usually be required
in Linux, and in Windows you will have to right-click on install.bat
and select "Run as administrator".

If "make show_prefix" does not show the correct folders, or the steps
above don't work for you for any other reason, then you can install
the library manually by following the instructions below.


== Manual installation ==

If you have the source repository of the library instead of a binary
distribution, you will need to build the library (.a) files by running
"make" and also copy all the files in the "src" subfolder into the
"pololu" subfolder.

Next, copy libpololu_*.a into your avr-gcc "lib" subfolder.

Finally, copy the entire "pololu" subfolder into your avr-gcc
"include" subfolder.

You are now ready to use the Pololu AVR library.


== Manually installing special files for Atmel Studio 7 ==

If you are using Atmel Studio 7 in Windows, then you might want to
install our Atmel Studio 7 templates so you can easily make new
projects for Pololu boards.  These files are normally installed by the
Pololu AVR Library Installer for Windows, but you can do it manually
if you need to.

You will need to have a binary distribution instead of a copy of the
source repository.

To install the templates, simply double click on

  atmel_studio\extension.vsix

and follow the instructions to install the extension into Atmel
Studio 7.


== Using the Pololu AVR Library ==

For more information about the library, please see the Pololu AVR
C/C++ Library User's Guide:

  http://www.pololu.com/docs/0J20


== Arduino IDE ==

Parts of the Pololu AVR Library can be used in the Arduino IDE.  For
more information, see "Programming Orangutans and the 3pi Robot from
the Arduino environment":

  https://www.pololu.com/docs/0J17


