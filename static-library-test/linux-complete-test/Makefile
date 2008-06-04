CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -Os
CPP=/usr/bin/avr-g++
CC=/usr/bin/avr-gcc
OBJ2HEX=/usr/bin/avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_min

PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=test
OBJECT_FILES=test.o analog.o assert.o delay.o lcd.o

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.o: test.c analog.h
analog.o: analog.c

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
