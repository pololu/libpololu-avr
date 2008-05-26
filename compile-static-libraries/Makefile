CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_ORANGUTAN -ffunction-sections -Os
CPP=/usr/bin/avr-g++
CC=/usr/bin/avr-gcc
LIBRARY_OBJECT_FILES=OrangutanMotors.o OrangutanBuzzer.o OrangutanPushbuttons.o OrangutanLCD.o \
	OrangutanDelay.o OrangutanLEDs.o
OBJ2HEX=/usr/bin/avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lorangutan
PORT=/dev/ttyUSB0
AVRDUDE=/usr/bin/avrdude
TARGET=test
all: $(TARGET).hex

liborangutan.a: $(LIBRARY_OBJECT_FILES)
	ar rs liborangutan.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm *.o *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.obj: test.o liborangutan.a
	$(CC) $(CFLAGS) test.o $(LDFLAGS) -o $@

program : $(TARGET).hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
