devices = atmega48 atmega168 atmega328

all:
	$(foreach device,$(devices),$(MAKE) -C devices/$(device); )

clean:
	$(foreach device,$(devices),$(MAKE) clean -C devices/$(device); )
