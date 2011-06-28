CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega328p -Os
CPP=avr-g++
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu_atmega328p
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_min

PORT=/dev/ttyACM0
AVRDUDE=avrdude
TARGET=test
OBJECT_FILES=test.o qtr.o assert.o motors.o pot.o battery.o pushbuttons.o outputs.o leds.o lcd.o demo.o
M3PI_OBJECT_FILES=test.o qtr.o assert.o motors.o pot.o battery.o pushbuttons.o outputs.o leds.o lcd.o demo.m3pi.o

all: $(TARGET).hex m3pi_$(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.o: test.c assert.h qtr.h motors.h pot.h battery.h pushbuttons.h outputs.h leds.h lcd.h

%.m3pi.o : %.c
	$(CC) $(CFLAGS) -DM3PI -c -o $@ $<

test.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

m3pi_test.obj: $(M3PI_OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ 
	
#program: $(TARGET).hex
#	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

