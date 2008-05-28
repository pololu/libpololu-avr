#ifndef OrangutanLineSensors_h
#define OrangutanLineSensors_h

class OrangutanLineSensors
{
 public:

  // Constructor
  OrangutanLineSensors();

  // Reads the five line sensors into an array. There must be space
  // for five values.  Example usage:
  // unsigned int sensor_values[5];
  // OrangutanLineSensors.readLineSensors(sensor_values);
  // ...
  // The values returned are between 0 and 1000.
  static void read(unsigned int *sensor_values);

  // Turn the IR LEDs off and on.  This is mainly for use by the
  // readLineSensors method, and calling these functions before or
  // after the reading the sensors will have no effect on the
  // readings, but you may wish to use these for testing purposes.

  static void emittersOff();
  static void emittersOn();
  
};

#endif
