PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_ORANGUTAN=$(INCLUDE)/orangutan
LIB=$(PREFIX)/lib

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_ORANGUTAN -ffunction-sections -Os
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
	Delay.o

#	OrangutanDelay/OrangutanDelay.o \
#	OrangutanLineSensors/OrangutanLineSensors.o

OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lorangutan -lm
# -Wl,-u,vfprintf -lprintf_flt
# -Wl,-u,vfprintf -lprintf_min
PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=test

liborangutan.a: $(LIBRARY_OBJECT_FILES)
	avr-ar rs liborangutan.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.obj: test.o liborangutan.a
	$(CC) $(CFLAGS) test.o $(LDFLAGS) -o $@

test-cpp.obj: test-cpp.o OrangutanLCD.o OrangutanLineSensors.o
	$(CC) $(CFLAGS) test-cpp.o OrangutanLCD.o OrangutanLineSensors.o $(LDFLAGS) -o $@

install: liborangutan.a
	install -d $(LIB)
	install -d $(INCLUDE_ORANGUTAN)
	install -t $(LIB) liborangutan.a
	install orangutan_install.h $(INCLUDE)/orangutan.h
	install -t $(INCLUDE_ORANGUTAN) orangutan/*.h
