devices = atmega48 atmega168 atmega328p

all:
	$(foreach device,$(devices),$(MAKE) -C devices/$(device); )

.PHONY: clean
clean:
	$(foreach device,$(devices),$(MAKE) clean -C devices/$(device); )
	$(foreach example,$(shell ls examples),$(MAKE) clean -C examples/$(example);)

# set the PREFIX to point to the location of avr-gcc
PREFIX=$(shell type avr-gcc | sed 's/\/bin\/avr-gcc//' | sed 's/avr-gcc is //')/avr
INCLUDE=$(PREFIX)/include
INCLUDE_POLOLU=$(INCLUDE)/pololu
LIB=$(PREFIX)/lib

.PHONY: install
install: $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -d $(LIB)
	install -d $(INCLUDE_POLOLU)
	install -t $(LIB) $(foreach device,$(devices),devices/$(device)/libpololu_$(device).a)
	install -t $(INCLUDE_POLOLU) pololu/*.h
	install -t $(INCLUDE_POLOLU) pololu/orangutan

examples_3pi := 3pi-demo-program 3pi-linefollower-pid 3pi-linefollower 3pi-mazesolver 3pi-serial-slave simple-test-3pi 
examples_orangutan := buzzer1 buzzer3 lcd2 pushbuttons1 analog2 buzzer2 lcd1 motors2 simple-test 3pi-serial-master

# The 48 examples are the only ones that will work on the mega48.
# They will also work on the orangutans, which could have either a 168
# or a 328 processor.

examples_atmega48 := analog1 motors1 led1
examples_atmega168 := $(examples_atmega48) $(examples_3pi) $(examples_orangutan)
examples_atmega328p := $(examples_atmega48) $(examples_3pi) $(examples_orangutan)

make_example = $(foreach example,$(value examples_$(device)), \
		mkdir -p examples_$(device)/$(example) && \
		cp examples/$(example)/*.[ch] examples_$(device)/$(example)/ && \
		cat examples/template_$(device).mk examples/$(example)/Makefile > examples_$(device)/$(example)/Makefile && \
		cat examples/$(example)/*.aps | sed s/<PART>.*</PART>/<PART>$(device)</PART>/ > examples_$(device)/$(example)/Makefile && \
		)

.PHONY: examples
examples:
	$(foreach device,$(devices),$(make_example)) test 0
