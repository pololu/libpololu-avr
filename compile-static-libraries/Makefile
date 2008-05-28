PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_ORANGUTAN=$(INCLUDE)/orangutan
LIB=$(PREFIX)/lib

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_ORANGUTAN -ffunction-sections -Os
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES1=\
	OrangutanMotors/OrangutanMotors.o \
	OrangutanBuzzer/OrangutanBuzzer.o \
	OrangutanPushbuttons/OrangutanPushbuttons.o \
	OrangutanLCD/OrangutanLCD.o \
	OrangutanDelay/OrangutanDelay.o \
	OrangutanLEDs/OrangutanLEDs.o \
	OrangutanLineSensors/OrangutanLineSensors.o

LIBRARY_OBJECT_FILES=$(addprefix ../Arduino/,$(LIBRARY_OBJECT_FILES1))

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
	rm -f *.o *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.obj: test.o liborangutan.a
	$(CC) $(CFLAGS) test.o $(LDFLAGS) -o $@

test-cpp.obj: test-cpp.o OrangutanLCD.o OrangutanLineSensors.o
	$(CC) $(CFLAGS) test-cpp.o OrangutanLCD.o OrangutanLineSensors.o $(LDFLAGS) -o $@

program : $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

install: liborangutan.a
	install -d $(LIB)
	install -d $(INCLUDE_ORANGUTAN)
	install -t $(LIB) liborangutan.a
	install orangutan_install.h $(INCLUDE)/orangutan.h
	install -t $(INCLUDE_ORANGUTAN) orangutan/*.h
