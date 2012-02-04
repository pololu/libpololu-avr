# NOTE: to compile with avr-gcc less than verson 4.2.3, you must
# remove the atmega328p from the list of target devices below:
devices := atmega48 atmega168 atmega328p atmega324p atmega644p atmega1284p atmega1284p_x2
mcu_atmega48 := atmega48
mcu_atmega168 := atmega168
mcu_atmega328p := atmega328p
mcu_atmega324p := atmega324p
mcu_atmega644p := atmega644p
mcu_atmega1284p := atmega1284p
mcu_atmega1284p_x2 := atmega1284p

LIBRARY_OBJECTS=\
	OrangutanAnalog \
	OrangutanBuzzer \
	OrangutanDigital \
	OrangutanLCD \
	OrangutanLEDs \
	OrangutanMotors \
	OrangutanPulseIn \
	OrangutanPushbuttons \
	OrangutanResources \
	OrangutanSerial \
	OrangutanServos \
	OrangutanSPIMaster \
	OrangutanTime \
	OrangutanSVP \
	OrangutanX2 \
	Pololu3pi \
	PololuQTRSensors \
	PololuWheelEncoders


SHELL = sh

# We need to do our recursive make with cd, since WinAVR does not support make -C.
# See WinAVR bug 1932584, "recursive make call fails"
.PHONY: library_files
library_files:
	@echo making library files
	$(foreach device,$(devices),cd devices/$(device) && $(MAKE) && cd ../.. &&) echo -n

# Change the path to allow make within sh to work: see WinAVR bug 1915456 "make ignores parameters when executed from sh"
PATH := $(shell echo $$PATH | sed 's/\(WinAVR-[0-9]*\)\/bin/\\1\/utils\/bin/g'):$(PATH)

LIBRARY_FILES := $(foreach device,$(devices),libpololu_$(device).a)

.PHONY: clean
clean:
	$(foreach device,$(devices),cd devices/$(device) && $(MAKE) clean && cd ../.. &&) echo -n
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
#
# Note: Unless you specify LIB and INCLUDE_POLOLU as described above,
# or you add the AVR Studio 5 toolchain to your path, running
# 'make install' will NOT install the library into the AVR Studio 5
# toolchain.  For Windows users, we recommend using the library's
# executable installer.

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

# Include additional Makefile rules that are only available if you have
# downloaded the actual source of the library (from github).
# Silently fail otherwise.
-include src.mk
