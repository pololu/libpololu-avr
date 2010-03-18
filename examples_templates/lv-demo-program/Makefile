DEVICE ?= atmega328p
AVRDUDE_DEVICE ?= m328p

CFLAGS=-g -Wall -mcall-prologues -mmcu=$(DEVICE) -Os
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

PORT ?= /dev/ttyUSB0
AVRDUDE=avrdude
TARGET=test

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.obj: %.o
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
