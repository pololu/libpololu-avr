PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_ORANGUTAN=$(INCLUDE)/orangutan
LIB=$(PREFIX)/lib

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_ORANGUTAN -ffunction-sections -Os
CPP=/usr/bin/avr-g++
CC=/usr/bin/avr-gcc
LIBRARY_OBJECT_FILES=OrangutanMotors.o OrangutanBuzzer.o OrangutanPushbuttons.o OrangutanLCD.o \
	OrangutanDelay.o OrangutanLEDs.o OrangutanLineSensors.o
OBJ2HEX=/usr/bin/avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lorangutan -Wl,-u,vfprintf -lprintf_flt -lm
PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=test

all: $(TARGET).hex

liborangutan.a: $(LIBRARY_OBJECT_FILES)
	ar rs liborangutan.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.obj: test.o liborangutan.a
	$(CC) $(CFLAGS) test.o $(LDFLAGS) -o $@

program : $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

install: liborangutan.a
	install -d $(LIB)
	install -d $(INCLUDE_ORANGUTAN)
	install -t $(LIB) liborangutan.a
	install orangutan_install.h $(INCLUDE)/orangutan.h
	install -t $(INCLUDE_ORANGUTAN) orangutan/*.h
