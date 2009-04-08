SRC=../../src
ZIPDIR=lib_zipfiles
SRC_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).src.zip
BIN_ZIPFILE=$(ZIPDIR)/libpololu-avr-$(shell date +%y%m%d).zip
HEX_ZIPFILE=$(ZIPDIR)/libpololu-avr-example-hex-files-$(shell date +%y%m%d).zip
ARDUINO_ZIPFILE=$(ZIPDIR)/PololuArdunoLibraries-$(shell date +%y%m%d).zip
ARDUINO_QTR_ZIPFILE=$(ZIPDIR)/PololuQTRSensors-$(shell date +%y%m%d).zip

CFLAGS=-g -Wall -mcall-prologues -mmcu=$(DEVICE) -DLIB_POLOLU -ffunction-sections -Os -I$(SRC)/include -fno-inline-small-functions
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES=\
	OrangutanMotors.o \
	OrangutanBuzzer.o \
	OrangutanPushbuttons.o \
	OrangutanLCD.o \
	OrangutanLEDs.o \
	OrangutanAnalog.o \
	PololuQTRSensors.o \
	Pololu3pi.o \
	PololuQTRSensors.o \
	OrangutanResources.o \
	OrangutanTime.o \
	OrangutanSerial.o

LIBRARY = libpololu_$(DEVICE).a

$(LIBRARY): $(LIBRARY_OBJECT_FILES)
	avr-ar rs $(LIBRARY) $(LIBRARY_OBJECT_FILES)

.SECONDEXPANSION:
%.o:$(SRC)/$$*/%.cpp
	$(CPP) $(CFLAGS) $(SRC)/$*/$< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj
	rm -rf examples/hex-files

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

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
