Orangutan Arduino Libraries 080604

Copyright (c) 2008-2011 Pololu Corporation. For more information, see

http://www.pololu.com/docs/0J17/5
http://www.pololu.com
http://forum.pololu.com

You may freely modify and share this code, as long as you keep this notice intact (including the three links above).  Licensed under the Creative Commons BY-SA 3.0 license:

http://creativecommons.org/licenses/by-sa/3.0/

Disclaimer: To the extent permitted by law, Pololu provides this work without any warranty.  It might be defective, in which case you agree to be responsible for all resulting costs and damages.


== Included Libraries ===
OrangutanAnalog
OrangutanBuzzer
OrangutanLCD
OrangutanLEDs
OrangutanMotors
OrangutanPushbuttons


== Installation & Usage Instructions ==

If you currently have an older version of our Orangutan Arduino Libraries, your first step should be to delete all of the Orangutan____ directories from your arduino-0011/hardware/libraries directory. If you don't perform this step, the newer version of the libraries might not get compiled.

Extract this archive to your arduino-0011/hardware/libraries directory.

You should now be able to use these libraries in your sketches by selecting Sketch > Import Library > Orangutan_____ from your Arduino 0011 IDE (or simply type #include <Orangutan_____.h> at the top of your sketch). Note that you might need to restart your Arduino 0011 IDE before it sees the new libraries. If you do not need the functionality of a specific library, you should leave it out of your sketch or, if you've already added it, remove it from your sketch by deleting the #include line associated with it from the top of your sketch. Included libraries will take up program space, even if you don't use them in your program.

Each library comes with at least one example sketch that demonstrates how the library can be used. To load an example sketch, open the Arduino IDE and go to File > Sketchbook > Examples > Library-Orangutan_____. The example sketches all have names that end with Example.


== Change Log ==

080604 - 
* Several bug fixes and minor improvements.
* It is now possible to play melodies directly out of program memory using the OrangutanBuzzer::playFromProgramSpace() method.
* Added support for creating custom LCD characters and an example sketch that demonstrates how this can be done.
