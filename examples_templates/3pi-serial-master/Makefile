CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -Os
CC=/usr/bin/avr-gcc
OBJ2HEX=/usr/bin/avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu

PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=master
OBJECT_FILES=master.o

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

slave.o: slave.c

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
