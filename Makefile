devices = atmega48 atmega168 atmega328p

all:
	$(foreach device,$(devices),$(MAKE) -C devices/$(device); )

clean:
	$(foreach device,$(devices),$(MAKE) clean -C devices/$(device); )

PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib

install: $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan

