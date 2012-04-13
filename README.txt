Pololu AVR Library

Copyright (c) 2008-2012 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J20
http://www.pololu.com
http://forum.pololu.com


== Features ===

The Pololu AVR Library is a collection of support functions for
programming AVR-based Pololu products or for using Pololu products
with AVRs.  It is designed for use with the free avr-gcc compiler,
which is included with AVR Studio 5.  Most of the library can also be
used together with the Arduino environment.

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
and tutorials for using AVR Studio 5 can be found in the Pololu AVR
C/C++ Library User's Guide:

  http://www.pololu.com/docs/0J20

The general instructions below apply to all development environments.


== Prerequisites ==

The free avr-gcc compiler, avr-libc, and other associated tools must
be installed before the Pololu AVR library.  Once again, please see
the Pololu AVR C/C++ Library User's Guide for detailed instructions
for Windows, Linux, and Mac OS X.

To program the Pololu Orangutan or 3pi, we recommend the Pololu
USB AVR Programmer.  Follow the instructions at

  http://www.pololu.com/docs/0J36

to install it on your computer.


== Distributions of the Pololu AVR Library ==

The source code of the Pololu AVR Library is available as a git
repository hosted on github:

  http://github.com/pololu/libpololu-avr

The source repository is used to generate a binary distribution that
has compiled binaries (.a files), headers (.h files), examples for
every supported device, AVR Studio 5 templates, and more.  The binary
distribution contains the source code of the libraries, but it does
NOT include every file from the source repository.  The binary
distributions can be downloaded as zip files.

You can tell what type of distribution you have by looking at what
files are present in it.  The source repository will have a src.mk
file, while the binary distribution will not.

For the convnience of Windows users, the binary distribution is
packaged in an executable installer called the Pololu AVR Library
Installer for Windows.  This installer can be downloaded by itself and
is also included in the Pololu AVR Development Bundle, a large
download that installs all of the prerequisites you will need for
programming AVRs in Windows.

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
* C:\Program Files (x86)\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.3.2.31\AVRToolChain\avr
* C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\avr
* C:\Program Files (x86)\Atmel\AVR Studio 5.0\AVR Toolchain\avr
* C:\WinAVR-20100110\avr
* /usr/lib/avr
* /usr/avr

In this folder you will find an "include" folder that has AVR header
files such as avr/io.h and a "lib" folder that has AVR libraries such
as lib/libc.a.

In the library's top-level folder, type "make show_prefix".  The
Makefile will tell you where it thinks your avr-gcc files are.  If the
folder names displayed are correct, then you can run "make install" to
install the library files.  You will need to run this command with the
correct permissions, so "sudo make install" will usually be required
in Linux, and in Windows Vista/7 you will have to right-click on
install.bat and select "Run as administrator".

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


== Manually installing special files for AVR Studio 5 ==

If you are using AVR Studio 5 in Windows, then you might want to
install our AVR Studio 5 templates so you can easily make new projects
for Pololu boards, and you might want to install the STK500 XML files
needed to program these boards from AVR Studio 5.  These files are
normally installed by the Pololu AVR Library Installer for Windows,
but you can do it manually if you need to.

You will need to have a binary distribution instead of a copy of the
source repository.

To install the templates, copy the the zip files in the library's

  avr_studio_5\templates

folder to the

  projecttemplates\Pololu

folder in your AVR Studio 5 installation.  The "projecttemplates"
folder should exist already, but you might need to create the "Pololu"
folder.  Next, close AVR Studio 5 and rename its ProjectTemplatesCache
folder to "ProjectTemplatesCache.bak".  The next time you start AVR
Studio 5, the templates should be available in the "New Project"
dialog, in the "Pololu" subcategory of "AVR GCC".

To install the STK500 XML files, close AVR Studio 5 and copy the XML
files in the library's

  avr_studio_5\stk500_xml

folder to the

  tools\STK500\xml

folder in your AVR Studio 5 installation.  The next time you start AVR
Studio 5, you should see ATmega328P, ATmega1284P, and ATmega644P in
the "Device" dropdown box of the "AVR Programming" dialog box when an
STK500 is selected.


== Using the Pololu AVR Library ==

For detailed instructions on using the library, please see our
instructions at:

  http://www.pololu.com/docs/0J20

