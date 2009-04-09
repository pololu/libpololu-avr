devices = atmega48 atmega168 atmega328p

all:
	$(foreach device,$(devices),$(MAKE) -C devices/$(device); )

clean:
	$(foreach device,$(devices),$(MAKE) clean -C devices/$(device); )

# set the PREFIX to point to the location of avr-gcc
PREFIX=$(shell type avr-gcc | sed 's/\/bin\/avr-gcc//' | sed 's/avr-gcc is //')/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib

install: $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan

