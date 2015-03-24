Pololu Orangutan and 3pi Robot add-on for the Arduino IDE

These files allow you to program Pololu Orangutan robot controllers
and the 3pi Robot from the Arduino IDE.

These files were made for version 1.6.x of the Arduino IDE.  If you
have an older version, you might need to update it.

For more information, see:

  https://www.pololu.com/docs/0J17


== Installation ==

To install the add-on, copy the "libpololu-arduino" directory from
this archive into the "hardware" subfolder of your Arduino sketchbook
location.  The Arduino sketchbook location is typically in your
Documents folder in a subfolder named "Arduino".  You can see the
sketchbook location in the Arduino IDE Preferences dialog, which is
available from the File menu.

For example, if you are using Windows and you have not changed the
sketchbook location, the "libpololu-arduino" folder would be copied
to:

  C:\Users\<username>\Documents\Arduino\hardware\libpololu-arduino

Next, restart the Arduino IDE.

After installing, you should see several new features inside the
Arduino IDE:

* You should see entries for the Orangutans in the Board menu.
* You should see an entry for the Pololu USB AVR Programmer in the
  Programmer menu.
* You should see several new entries in the "Examples" menu, like
  "OrangutanMotors" and "OrangutanLCD".

If you do not see these new features, then the add-on was probably not
installed correctly and you should retry the installation instructions
above.


== Change Log ==

2015 Mar 24 -
* Fixed a bug in PololuWheelEncoders that was causing it to lose half
  of the counts.  This bug was introduced in 2012.

2015 Feb 18 -
* Packaged the libraries as an Arduino hardware add-on for Arduino
  1.6.0, which also provides needed entries in the Board and
  Programmer menus.

2011 Dec 21 -
* Updated the libraries to be compatible with the Arduino 1.0
  environment.  These changes mean that the libraries will not work
  correctly under versions prior to Arduino 1.0.

2010 Dec 15 -
* Updated the libraries to be compatible with the Arduino 0021
  environment.  These changes mean that the libraries will not work
  correctly under versions prior to Arduino 0019.
* Made C functions accessible to Arduino programs.
* Included the PololuWheelEncoders library.

2008 Sep 29 -
* Updated the library so that it would be compatible with the new
  Arduino-0012 environment.  These changes mean that the library will
  not work correctly under older versions of the Arduino environment.

2008 Aug 25 -
* Included the Pololu3pi library, which makes it easier to interface
  with the Pololu 3pi robot.

2008 Jun 04 -
* Several bug fixes and minor improvements.
* It is now possible to play melodies directly out of program memory
  using the OrangutanBuzzer::playFromProgramSpace() method.
* Added support for creating custom LCD characters and an example
  sketch that demonstrates how this can be done.
