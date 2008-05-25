CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega168
CPP=/usr/bin/avr-g++
CC=/usr/bin/avr-gcc
C_FILES=../OrangutanMotors/OrangutanMotors.cpp
OBJ2HEX=/usr/bin/avr-objcopy 

liborangutan.a: liborangutan.o
	ar rs liborangutan.a liborangutan.o

liborangutan.o:../OrangutanMotors/OrangutanMotors.cpp
	$(CPP) $(CFLAGS) $(C_FILES) -c -o liborangutan.o

clean:
	rm *.o *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

test.obj: test.o liborangutan.a
	$(CC) $(CFLAGS) test.o -L. -lorangutan -o $@
