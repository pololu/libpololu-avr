CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -Os
CPP=avr-g++
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_min

PORT=/dev/ttyUSB0
AVRDUDE=avrdude
TARGET=test
OBJECT_FILES=test.o qtr.o assert.o motors.o pot.o battery.o pushbuttons.o outputs.o leds.o lcd.o

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.o: test.c assert.h qtr.h motors.h pot.h battery.h pushbuttons.h outputs.h leds.h lcd.h

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
