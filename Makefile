# NOTE: to compile with avr-gcc less than verson 4.2.3, you must
# remove the atmega328p from the list of target devices below:
devices := atmega48 atmega168 atmega328p atmega324p atmega1284p

LIBRARY_OBJECTS=\
	OrangutanAnalog \
	OrangutanBuzzer \
	OrangutanDigital \
	OrangutanLCD \
	OrangutanLEDs \
	OrangutanSVP \
	OrangutanSPIMaster \
	OrangutanMotors \
	OrangutanPulseIn \
	OrangutanPushbuttons \
	OrangutanResources \
	OrangutanSerial \
	OrangutanServos \
	OrangutanTime \
	Pololu3pi \
	PololuQTRSensors \
	PololuWheelEncoders


SHELL = sh

# We need to do our recursive make with cd, since WinAVR does not support make -C.
# See WinAVR bug 1932584, "recursive make call fails"
.PHONY: library_files
library_files:
	echo making library files
	echo $(SHELL)
	$(foreach device,$(devices),cd devices/$(device) ; $(MAKE) ; cd ../.. ;)

# Change the path to allow make within sh to work: see WinAVR bug 1915456 "make ignores parameters when executed from sh"
PATH := $(shell echo $$PATH | sed 's/\(WinAVR-[0-9]*\)\/bin/\\1\/utils\/bin/g'):$(PATH)

LIBRARY_FILES := $(foreach device,$(devices),libpololu_$(device).a)

SHELL=sh

.PHONY: clean
clean:
	echo $(SHELL)
	$(foreach device,$(devices),cd devices/$(device) ; $(MAKE) clean ; cd ../.. ;)
	if [ -e examples_templates -a -e examples ]; then \
		for dir in examples/*/*; \
		do \
			$(MAKE) clean -C $$dir; \
		done; \
		rm -rf examples; \
	fi
	if [ -e examples_templates ]; then \
		for dir in examples_templates/*/; \
		do \
			$(MAKE) clean -C $$dir; \
		done; \
	fi
	rm -f $(LIBRARY_FILES)

# "make install" basically just copies the .a and files to the lib directory,
# and the header files to the include directory.  The tricky thing is
# determining which directories those are in this makefile.
#
# By default, this makefile will install .a files in $(LIB)
# and header files in $(LIB)/../include/pololu, where LIB is determined
# by running `avr-gcc -print-search-dirs`, looking on the libraries
# line, taking the last directory listed.
#
# This seems to be a good choice on most systems because it points to
# a path that does not include the avr-gcc version number.
#
# You can check what directories this makefile will use by running
#    make show_prefix
#
# You can override this behavior by inserting a line below that manually
# sets INCLUDE_POLOLU and LIB to a directory of your choice.
# For example, you could uncomment these lines:
#   LIB := /usr/lib/avr/lib
#   INCLUDE_POLOLU := /usr/lib/avr/include


# Figure out what operating system we are running in.
UNAME := $(shell uname)
ifeq ($(findstring NT, $(UNAME)), NT)
  WINDOWS=true
endif
ifeq ($(findstring MINGW, $(UNAME)), MINGW)
  WINDOWS=true
endif

ifeq ($(origin LIB), undefined)
  ifdef WINDOWS
    # Directories are separated with ;
    LIB := $(shell avr-gcc -print-search-dirs | grep -e "^libraries" | sed 's/.*;//')
  else
    # Directories are separated with :
    LIB := $(shell avr-gcc -print-search-dirs | grep -e "^libraries" | sed 's/.*://')
  endif
endif

INCLUDE_POLOLU ?= $(LIB)/../include/pololu

# Normalize the directory names so they don't have ".." in them.
# Doesn't work in Windows.
ifndef WINDOWS
  LIB := $(abspath $(LIB))
  INCLUDE_POLOLU := $(abspath $(INCLUDE_POLOLU))
endif

INSTALL_FILES := install -m=r--

.PHONY: show_prefix
show_prefix:
	@echo The Pololu AVR Library object files \(.a\) will be installed in $(LIB)
	@echo The header files \(.h\) will be installed in $(INCLUDE_POLOLU)

.PHONY: install
install: $(LIBRARY_FILES)
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install $(foreach device,$(devices),libpololu_$(device).a) $(LIB)
	$(INSTALL_FILES) pololu/*.h $(INCLUDE_POLOLU)
	for OrangutanObject in $(LIBRARY_OBJECTS); do install -d $(INCLUDE_POLOLU)/$$OrangutanObject ; $(INSTALL_FILES) src/$$OrangutanObject/*.h $(INCLUDE_POLOLU)/$$OrangutanObject ; done
	install -d $(INCLUDE_POLOLU)/OrangutanResources/include
	$(INSTALL_FILES) src/OrangutanResources/include/*.h $(INCLUDE_POLOLU)/OrangutanResources/include
	$(INSTALL_FILES) pololu/orangutan $(INCLUDE_POLOLU)
	@echo "Installation is complete."

# We make all of the examples from templates in the examples_templates
# directory (which is not distributed), by running a bunch of commands
# concatenated together with &&.

examples_3pi := 3pi-demo-program 3pi-linefollower-pid 3pi-linefollower 3pi-mazesolver 3pi-serial-slave
examples_orangutan := buzzer1 buzzer3 lcd2 pushbuttons1 analog3 buzzer2 lcd1 motors2 simple-test wheel_encoders1 digital1
examples_168_328p := analog2 3pi-serial-master SV-xx8_demo_program LV-xx8_demo_program serial1
examples_svp := svp1 svp-demo-program svp-eight-servo svp-sixteen-servo svp-one-servo

# The 48 examples are the only ones that will work on the mega48.
# They will also work on the orangutans, which could have either a 168
# or a 328 processor.

examples_atmega48 := digital1 analog1 motors1 led1 simple_servo_control
examples_atmega168 := $(examples_atmega48) $(examples_3pi) $(examples_orangutan) $(examples_168_328p)
examples_atmega328p := $(examples_atmega48) $(examples_3pi) $(examples_orangutan) $(examples_168_328p)
examples_atmega324p := $(examples_atmega48) $(examples_orangutan) $(examples_svp)
examples_atmega1284p := $(examples_atmega324p)

example_template = examples_templates/$(example)
example_dir = examples/$(device)/$(example)
hex_dir = examples/$(device)/hex_files

make_example = $(foreach example,$(value examples_$(device)), \
		mkdir -p $(example_dir) && \
		cp -a $(example_template)/*.[ch] $(example_dir)/ && \
		cat examples_templates/template_$(device).mk $(example_template)/Makefile > $(example_dir)/Makefile && \
		cat $(example_template)/*.aps | sed 's/<PART>[^<]*<\/PART>/<PART>$(device)<\/PART>/' \
			| sed 's/<LIB>libpololu[^<]*\.a<\/LIB>/<LIB>libpololu_$(device).a<\/LIB>/' > $(example_dir)/$(example).aps && \
		)

.PHONY: examples
examples:
	$(foreach device,$(devices),$(make_example)) test 0

make_hex_files = mkdir -p $(hex_dir) && \
	$(foreach example,$(value examples_$(device)), \
	make -C $(example_dir) && \
	cp -a $(example_dir)/*.hex $(hex_dir)/$(example).hex && \
	)

.PHONY: hex_files
hex_files: examples
	$(foreach device,$(devices),$(make_hex_files)) test 0

# The following code creates the zip file.
ZIPDIR=lib_zipfiles
DATE := $(shell date +%y%m%d)
LIB_ZIPFILE := $(ZIPDIR)/libpololu-avr-$(DATE).zip
HEX_ZIPFILE := $(ZIPDIR)/libpololu-avr-example-hex-files-$(DATE).zip
ARDUINO_ZIPFILE := $(ZIPDIR)/PololuArduinoLibraries-$(DATE).zip
ARDUINO_QTR_ZIPFILE := $(ZIPDIR)/PololuQTRSensors-$(DATE).zip

ZIP_EXCLUDES := \*.o .svn/\* \*/.svn/\* \*.hex \*.zip libpololu-avr/arduino_zipfiles/ arduino_zipfiles/\* \*/lib_zipfiles/\* \*.elf \*.eep \*.lss \*.o.d libpololu-avr/libpololu-avr/\* libpololu-avr/extra/\* libpololu-avr/graphics/\* \*.map \*/test/\* \*/ArduinoReadMe.txt \*/examples_templates/\* \*/README-Arduino.txt

ARDUINO_EXCLUDES := libpololu-arduino/OrangutanTime/\* libpololu-arduino/OrangutanSerial/\* libpololu-arduino/OrangutanSVP/\* libpololu-arduino/include/\*
NON_ARDUINO_EXCLUDES := libpololu-avr/src/\*/examples/\* libpololu-avr/src/\*/keywords.txt

.PHONY: zip
zip: library_files examples hex_files arduino_zip
	rm -f libpololu-avr
	mkdir -p $(ZIPDIR)
	rm -f $(LIB_ZIPFILE)
	rm -f $(HEX_ZIPFILE)
	yes | rm -Rf $(foreach object, $(LIBRARY_OBJECTS), ./pololu/$(object))
	cp -R $(foreach object, $(LIBRARY_OBJECTS), ./src/$(object)) ./pololu
	yes | rm -Rf ./pololu/*/*.cpp ./pololu/*/examples ./pololu/*/private ./pololu/*/keywords.txt
	ln -s . libpololu-avr
	zip -rq $(LIB_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES) $(NON_ARDUINO_EXCLUDES)
	zip -rq $(LIB_ZIPFILE) libpololu-avr/examples/*/hex_files/*.hex
	rm libpololu-avr
	yes | rm -Rf $(foreach object, $(LIBRARY_OBJECTS), ./pololu/$(object))

arduino_zip:
	mkdir -p $(ZIPDIR)
	rm -f $(ARDUINO_ZIPFILE)
	rm -f $(ARDUINO_QTR_ZIPFILE)
	ln -s src libpololu-arduino
	zip -rq $(ARDUINO_ZIPFILE) libpololu-arduino -x $(ZIP_EXCLUDES) -x $(ARDUINO_EXCLUDES)
	zip -rq $(ARDUINO_ZIPFILE) libpololu-arduino/README-Arduino.txt
	rm libpololu-arduino
	ln -s src/PololuQTRSensors .
	zip -rq $(ARDUINO_QTR_ZIPFILE) PololuQTRSensors -x $(ZIP_EXCLUDES) -x $(ARDUINO_EXCLUDES)
	rm PololuQTRSensors
