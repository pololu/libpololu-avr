PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_POLOLU -ffunction-sections -Os
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES=\
	../Arduino/OrangutanMotors/OrangutanMotors.o \
	../Arduino/OrangutanBuzzer/OrangutanBuzzer.o \
	../Arduino/OrangutanPushbuttons/OrangutanPushbuttons.o \
	../Arduino/OrangutanLCD/OrangutanLCD.o \
	../Arduino/OrangutanLEDs/OrangutanLEDs.o \
	../Arduino/OrangutanAnalog/OrangutanAnalog.o \
	../Arduino/PololuQTRRC/PololuQTRRC.o \
	../Arduino/Pololu3pi/Pololu3pi.o \
	Delay.o

#	OrangutanDelay/OrangutanDelay.o \
#	OrangutanLineSensors/OrangutanLineSensors.o

OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lpololu -lm
# -Wl,-u,vfprintf -lprintf_flt
# -Wl,-u,vfprintf -lprintf_min
PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=test

libpololu.a: $(LIBRARY_OBJECT_FILES)
	avr-ar rs libpololu.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

install: libpololu.a
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) libpololu.a
	install pololu_install.h $(INCLUDE)/pololu.h
	install -t $(INCLUDE_POLOLU) pololu/*.h
