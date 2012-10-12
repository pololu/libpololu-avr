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
OBJECT_FILES=test.o qtr.o assert.o motors.o pot.o battery.o pushbuttons.o outputs.o leds.o lcd.o demo.o serial-slave.o
M3PI_OBJECT_FILES=test.m3pi.o qtr.o assert.o motors.o pot.o battery.o pushbuttons.o outputs.o leds.o lcd.o demo.m3pi.o serial-slave.o

all: $(TARGET).hex m3pi_$(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.o: test.h test.c assert.h qtr.h motors.h pot.h battery.h pushbuttons.h outputs.h leds.h lcd.h serial-slave.h

%.m3pi.o : %.c
	$(CC) $(CFLAGS) -DM3PI -c -o $@ $<

test.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

m3pi_test.obj: $(M3PI_OBJECT_FILES)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p m328p -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex

program_m3pi: m3pi_$(TARGET).hex
	$(AVRDUDE) -p m328p -c avrisp2 -P $(PORT) -U flash:w:m3pi_$(TARGET).hex
