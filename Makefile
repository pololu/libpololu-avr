PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib
ZIPDIR=lib_zipfiles
SRC_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).src.zip
BIN_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).zip

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_POLOLU -ffunction-sections -Os
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES=\
	Arduino/OrangutanMotors/OrangutanMotors.o \
	Arduino/OrangutanBuzzer/OrangutanBuzzer.o \
	Arduino/OrangutanPushbuttons/OrangutanPushbuttons.o \
	Arduino/OrangutanLCD/OrangutanLCD.o \
	Arduino/OrangutanLEDs/OrangutanLEDs.o \
	Arduino/OrangutanAnalog/OrangutanAnalog.o \
	Arduino/PololuQTRSensors/PololuQTRSensors.o \
	Arduino/Pololu3pi/Pololu3pi.o \
	Arduino/PololuQTRSensors/PololuQTRSensors.o \
	OrangutanDelay.o

OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lpololu -lm

libpololu.a: $(LIBRARY_OBJECT_FILES)
	avr-ar rs libpololu.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

install: libpololu.a
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) libpololu.a
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan

ZIP_EXCLUDES=\*.o .svn/\* \*/.svn/\* \*.hex \*.zip arduino_zipfiles/\* \*/lib_zipfiles/\* \*.elf \*.eep \*.lss \*.o.d libpololu-avr/libpololu-avr/\* libpololu-avr/extra/\*

zip: libpololu.a
	mkdir -p $(ZIPDIR)
	rm -f $(SRC_ZIPFILE)
	rm -f $(BIN_ZIPFILE)
	ln -s . libpololu-avr
	zip -rq $(SRC_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES) \*.a
	rm libpololu-avr
	ln -s extra/src libpololu-avr
	zip -rq $(SRC_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES)
	rm libpololu-avr
	ln -s . libpololu-avr
	zip -rq $(BIN_ZIPFILE) libpololu-avr/libpololu.a libpololu-avr/pololu -x $(ZIP_EXCLUDES)
	rm libpololu-avr
	ln -s extra/bin libpololu-avr
	zip -rq $(BIN_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES)
	rm libpololu-avr

