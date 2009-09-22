SRC=../../src
TOPLEVEL=../../

CFLAGS=-g -Wall -mcall-prologues -mmcu=$(DEVICE) -DLIB_POLOLU -ffunction-sections -Os -I$(SRC)/include -I$(TOPLEVEL)
CPP=avr-g++
CC=avr-gcc

LIBRARY_OBJECT_FILES=\
	OrangutanAnalog.o \
	OrangutanBuzzer.o \
	OrangutanDigital.o \
	OrangutanLCD.o \
	OrangutanLEDs.o \
	OrangutanSPIMaster.o \
	OrangutanSVP.o \
	OrangutanMotors.o \
	OrangutanPulseIn.o \
	OrangutanPushbuttons.o \
	OrangutanResources.o \
	OrangutanSerial.o \
	OrangutanServos.o \
	OrangutanTime.o \
	Pololu3pi.o \
	PololuQTRSensors.o \
	PololuWheelEncoders.o

LIBRARY = ../../libpololu_$(DEVICE).a

$(LIBRARY): $(LIBRARY_OBJECT_FILES)
	avr-ar rs $(LIBRARY) $(LIBRARY_OBJECT_FILES)

.SECONDEXPANSION:
%.o:$(SRC)/$$*/%.cpp $(SRC)/$$*/%.h
	$(CPP) $(CFLAGS) $(SRC)/$*/$< -c -o $@

clean:
	rm -f $(LIBRARY_OBJECT_FILES) *.a *.hex *.obj
	rm -rf examples/hex-files

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

