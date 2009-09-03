PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib
ZIPDIR=lib_zipfiles
SRC_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).src.zip
BIN_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).zip
HEX_ZIPFILE=$(ZIPDIR)/libpololu-avr-example-hex-files-$(shell date +%y%m%d).zip
ARDUINO_ZIPFILE=$(ZIPDIR)/PololuArdunoLibraries-$(shell date +%y%m%d).zip
ARDUINO_QTR_ZIPFILE=$(ZIPDIR)/PololuQTRSensors-$(shell date +%y%m%d).zip

CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -DLIB_POLOLU -ffunction-sections -Os
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES=\
	src/*/*.o

OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -L. -lpololu -lm

libpololu.a: $(LIBRARY_OBJECT_FILES)
	avr-ar rs libpololu.a $(LIBRARY_OBJECT_FILES)

%.o:%.cpp
	$(CPP) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj
	rm -rf examples/hex-files

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

install: libpololu.a
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) libpololu.a
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan

ZIP_EXCLUDES=\*.o .svn/\* \*/.svn/\* \*.hex \*.zip libpololu-avr/arduino_zipfiles/ arduino_zipfiles/\* \*/lib_zipfiles/\* \*.elf \*.eep \*.lss \*.o.d libpololu-avr/libpololu-avr/\* libpololu-avr/extra/\* libpololu-avr/graphics/\* \*.map \*/test/\* \*/ArduinoReadMe.txt

ARDUINO_EXCLUDES=libpololu-arduino/OrangutanTime/\* libpololu-arduino/OrangutanSerial/\*

zip: libpololu.a hex-files
	mkdir -p $(ZIPDIR)
	rm -f $(SRC_ZIPFILE)
	rm -f $(BIN_ZIPFILE)
	rm -f $(ARDUINO_ZIPFILE)
	rm -f $(ARDUINO_QTR_ZIPFILE)
	rm -f $(HEX_ZIPFILE)
	ln -s extra/src libpololu-avr
	zip -rq $(SRC_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES)
	rm libpololu-avr
	ln -s . libpololu-avr
	zip -rq $(SRC_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES) \*.a
	zip -rq $(SRC_ZIPFILE) libpololu-avr/examples/hex-files/*.hex
	rm libpololu-avr
	ln -s extra/bin libpololu-avr
	zip -rq $(BIN_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES)
	rm libpololu-avr
	ln -s . libpololu-avr
	zip -rq $(BIN_ZIPFILE) libpololu-avr/README.txt libpololu-avr/libpololu.a libpololu-avr/pololu libpololu-avr/examples -x $(ZIP_EXCLUDES)
	zip -rq $(BIN_ZIPFILE) libpololu-avr/examples/hex-files/*.hex
	rm libpololu-avr
	ln -s src libpololu-arduino
	zip -rq $(ARDUINO_ZIPFILE) README-Arduino.txt libpololu-arduino -x $(ZIP_EXCLUDES) -x $(ARDUINO_EXCLUDES)
	rm libpololu-arduino
	ln -s src/PololuQTRSensors .
	zip -rq $(ARDUINO_QTR_ZIPFILE) PololuQTRSensors -x $(ZIP_EXCLUDES) -x $(ARDUINO_EXCLUDES)
	rm PololuQTRSensors

hex-files:
	mkdir -p examples/hex-files
	for i in `find examples/* -prune -type d -not -name hex-files`; do cd $$i; make clean; make; mv *.hex ../hex-files/`basename $$i`.hex; cd ../..; done
