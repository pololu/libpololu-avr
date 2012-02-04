Pololu AVR Libraries for the Arduino IDE

Copyright (c) 2008-2011 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J17/5
http://www.pololu.com
http://forum.pololu.com

You may freely modify and share this code, as long as you keep this
notice intact (including the three links above).  Licensed under the
Creative Commons BY-SA 3.0 license:

http://creativecommons.org/licenses/by-sa/3.0/

Disclaimer: To the extent permitted by law, Pololu provides this work
without any warranty.  It might be defective, in which case you agree
to be responsible for all resulting costs and damages.


== Included Libraries ===
OrangutanAnalog
OrangutanBuzzer
OrangutanDigital
OrangutanLCD
OrangutanLEDs
OrangutanMotors
OrangutanPushbuttons
OrangutanResources
Pololu3pi
PololuQTRSensors
PololuWheelEncoders


== Installation & Usage Instructions ==

Note: the latest version of this library has been tested with and will
only work with Arduino 1.0 or newer.  If you have an older version of
the Arduino environment, please update to version 1.0 before using
this library.

If you currently have an older version of our Orangutan Arduino
Libraries, your first step should be to delete all of the
Orangutan____ and Pololu____ directories from your 
arduino-1.0/libraries directory (arduino-____/hardware/libraries for
some older versions of the Arduino environment). If you don't perform
this step, the newer version of the libraries might not be compiled.

Extract this archive to your arduino-1.0/libraries directory.

You should now be able to use these libraries in your sketches by
selecting Sketch > Import Library > Orangutan_____ from your Arduino
1.0 IDE (or simply type #include <Orangutan_____.h> at the top of
your sketch). Note that you might need to restart your Arduino
IDE before it sees the new libraries. If you do not need the
functionality of a specific library, you should leave it out of your
sketch or, if you've already added it, remove it from your sketch by
deleting the #include line associated with it from the top of your
sketch. Included libraries will take up program space, even if you
don't use them in your program.

Each library comes with at least one example sketch that demonstrates
how the library can be used. To load an example sketch, open the
Arduino IDE and go to File > Examples > Orangutan_____. The example
sketches all have names that end with Example.


== Change Log ==

111221 -
* Updated the libraries to be compatible with the Arduino 1.0
  environment.  These changes mean that the libraries will not work
  correctly under versions prior to Arduino 1.0.

101215 -
* Updated the libraries to be compatible with the Arduino 0021
  environment.  These changes mean that the libraries will not work
  correctly under versions prior to Arduino 0019.
* Made C functions accessible to Arduino programs.
* Included the PololuWheelEncoders library.

080929 -
* Updated the library so that it would be compatible with the new
  Arduino-0012 environment.  These changes mean that the library will
  not work correctly under older versions of the Arduino environment.

080825 -
* Included the Pololu3pi library, which makes it easier to interface
  with the Pololu 3pi robot.

080604 - 
* Several bug fixes and minor improvements.
* It is now possible to play melodies directly out of program memory
  using the OrangutanBuzzer::playFromProgramSpace() method.
* Added support for creating custom LCD characters and an example
  sketch that demonstrates how this can be done.
