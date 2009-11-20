#!/usr/bin/ruby

# This program generates the Makefile.
# David read a lot about Makefile syntax and couldn't find a better way
# to do this.  The tricky thing is that we target names with variables
# in them, e.g. $(OBJ).$(DEVICE).o.  Make can't handle that well.

Dir.chdir(File.dirname(__FILE__)) # change working directory to this script's.
file = File.open('Makefile', 'w')

objects = %w(test analog assert delay lcd buzzer leds motors pushbuttons qtr)
devices = %w(168 324p)

file.puts <<END
CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -Os
CPP=avr-g++
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_min

DEVICES=#{devices.join(' ')}

PORT=/dev/ttyACM0
AVRDUDE=avrdude
TARGET=test
OBJECT_FILES=#{objects.collect{|x|x+'.$*.o'}.join(' ')}
HEXES=$(foreach device, $(DEVICES), $(TARGET).$(device).hex)

all: $(HEXES)

clean:
	rm -f *.o *.hex *.obj *.hex

# $< = prerequisite
# $@ = target
# $* = stem (percent)

$(HEXES): $(TARGET).%.hex: $(TARGET).%.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(foreach device, $(DEVICES), $(TARGET).$(device).obj): test.%.obj: #{objects.collect{|x|x+'.%.o'}.join(' ')}
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -lpololu_atmega$* -o $@
END

objects.each do |object|
  devices.each do |device|
    file.puts <<END
#{object}.#{device}.o: #{object}.c
	avr-gcc -mmcu=atmega#{device} -Wall -Os -c $< -o $@
END
  end
end

file.close