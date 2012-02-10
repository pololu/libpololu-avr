# This file contains GNU Make rules that are only available when you have
# the actual source of the Pololu AVR C/C++ Library (from github).
# Its main purpose is to generate the binary distribution zip file,
# which will include the Makefile but will not include this file.

device_specific_macro_atmega1284p_x2 := _X2_1284

# This section defines which examples are available for each devices.
examples_3pi := 3pi-demo-program 3pi-demo-or-serial-slave 3pi-linefollower-pid 3pi-linefollower 3pi-mazesolver 3pi-serial-slave
examples_orangutan := buzzer1 buzzer2 buzzer3 lcd1 lcd2 lcd3-hello-world pushbuttons1 pushbuttons2 motors2 simple-test wheel-encoders1 wdt servos-and-buzzer pulsein1 pulsein2 stepper-motor1
examples_168_328p := analog2 3pi-serial-master sv-demo-program lv-demo-program serial1
examples_svp := svp1 svp-demo-program svp-eight-servo svp-sixteen-servo svp-one-servo serial2
examples_x2 := digital1 serial2 x2-demo-program

# The 48 examples are the only ones that will work on the mega48.
# They will also work on the orangutans.

examples_atmega48 := digital1 analog1 motors1 led1 servo-control-using-delays servos1
examples_atmega168 := $(examples_atmega48) $(examples_3pi) $(examples_orangutan) $(examples_168_328p)
examples_atmega328p := $(examples_atmega48) $(examples_3pi) $(examples_orangutan) $(examples_168_328p)
examples_atmega324p := $(examples_atmega48) $(examples_orangutan) $(examples_svp)
examples_atmega644p := $(examples_atmega48) $(examples_orangutan) $(examples_x2)
examples_atmega1284p := $(examples_atmega324p)
examples_atmega1284p_x2 := $(examples_atmega644p)

# Define various directories.
example_dir = examples/$(device)/$(example)
hex_dir = examples/$(device)/hex_files

# examplss: A phony target that generates the source code in the
# examples directory from the source code in the examples_templates
# directory.
prepare_example = examples_templates/prepare.sh $$example $(device) $(mcu_$(device)) '$(device_specific_macro_$(device))'

.PHONY: examples
examples:
	$(foreach device,$(devices),(for example in $(examples_$(device)); do $(prepare_example); done) &&) test 0

# hex_files: A phony target that generates the hex files for the
# devices.  Uses one large command line.
make_hex_files = mkdir -p $(hex_dir) && \
	$(foreach example,$(value examples_$(device)), \
	make -C $(example_dir) && \
	cp -a $(example_dir)/*.hex $(hex_dir)/$(example).hex && \
	)

.PHONY: hex_files
hex_files: examples
	$(foreach device,$(devices),$(make_hex_files)) test 0

# Phony target that generates the AVR Studio 5 template zip files
# using the files in the templates directory.
.PHONY: avr_studio_5_templates
avr_studio_5_templates:
	avr_studio_5/generate_templates.sh

# Phony target that generates the XML files we need to add support
# for programming our AVRs to AVR Studio 5.
.PHONY: avr_studio_5_stk500_xml
avr_studio_5_stk500_xml:
	avr_studio_5/generate_stk500_xml.sh

# Phony target that generates the AVR Studio 5 extension
# (which only installs the templates, not the other parts of the library).
.PHONY: avr_studio_5_extension
avr_studio_5_extension: avr_studio_5_templates
	avr_studio_5/generate_extension.sh

# Phony target generates our NSIS installer script (installer.nsi)
# from the template (installer.nsi.template).
installer_nsi:
	sed "s/YYMMDD/$(DATE)/" installer.nsi.template > installer.nsi

# The following code creates the zip file.
ZIPDIR=lib_zipfiles
DATE := $(shell date +%y%m%d)
LIB_ZIPFILE := $(ZIPDIR)/libpololu-avr-$(DATE).zip
ARDUINO_ZIPFILE := $(ZIPDIR)/PololuArduinoLibraries-$(DATE).zip
ARDUINO_QTR_ZIPFILE := $(ZIPDIR)/PololuQTRSensors-$(DATE).zip

ZIP_EXCLUDES := \*~ \*.o \*/.git/\* \*/.gitignore .svn/\* \*/.svn/\* \*.hex libpololu-avr/src.mk libpololu-avr/avr_studio_5/\*.sh libpololu-avr/avr_studio_5/templates_src/\* \*.zip libpololu-avr/arduino_zipfiles/ arduino_zipfiles/\* \*/lib_zipfiles/\* \*.elf \*.eep \*.lss \*.o.d libpololu-avr/libpololu-avr/\* libpololu-avr/extra/\* libpololu-avr/graphics/\* libpololu-avr/templates/\* \*.map \*/test/\* \*/ArduinoReadMe.txt \*/examples_templates/\* \*/README-Arduino.txt libpololu-avr/avr_studio_5/\*Con*.xml libpololu-avr/avr_studio_5/extension.vsixmanifest\* extension.vsixmanifest\* libpololu-avr/avr_studio_5/\*.png libpololu-avr/avr_studio_5/ProjectTemplates/\* libpololu-avr/installer.nsi.template

ARDUINO_EXCLUDES :=  libpololu-arduino/OrangutanPulseIn/\* libpololu-arduino/OrangutanSerial/\* libpololu-arduino/OrangutanServos/\* libpololu-arduino/OrangutanSPIMaster/\* libpololu-arduino/OrangutanTime/\* libpololu-arduino/OrangutanSVP/\* libpololu-arduino/OrangutanX2/\* libpololu-arduino/include/\*
NON_ARDUINO_EXCLUDES := libpololu-avr/src/\*/examples/\* libpololu-avr/src/\*/keywords.txt

.PHONY: zip
zip: library_files examples hex_files arduino_zip avr_studio_5_templates avr_studio_5_stk500_xml avr_studio_5_extension installer_nsi
	rm -f libpololu-avr
	mkdir -p $(ZIPDIR)
	rm -f $(LIB_ZIPFILE)
	yes | rm -Rf $(foreach object, $(LIBRARY_OBJECTS), ./pololu/$(object))
	cp -R $(foreach object, $(LIBRARY_OBJECTS), ./src/$(object)) ./pololu
	yes | rm -Rf ./pololu/*/*.cpp ./pololu/*/examples ./pololu/*/private ./pololu/*/keywords.txt
	ln -s . libpololu-avr
	zip -rq $(LIB_ZIPFILE) libpololu-avr -x $(ZIP_EXCLUDES) $(NON_ARDUINO_EXCLUDES)
	zip -rq $(LIB_ZIPFILE) libpololu-avr/examples/*/hex_files/*.hex libpololu-avr/avr_studio_5/templates/*.zip
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

# Cleanup

clean: clean_examples

.PHONY: clean_examples
clean_examples:
	if [ -e examples_templates ]; then \
		rm -rf examples; \
		for dir in examples_templates/*/; \
		do \
			$(MAKE) clean -C $$dir; \
		done; \
	fi
