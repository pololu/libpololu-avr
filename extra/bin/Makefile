PREFIX=/usr/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib

install: libpololu.a
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) libpololu.a
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan
