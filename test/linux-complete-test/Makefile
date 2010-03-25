CFLAGS=-g -Wall -mcall-prologues -mmcu=atmega168 -Os
CPP=avr-g++
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_flt -lm
#LDFLAGS := $(LDFLAGS) -Wl,-u,vfprintf -lprintf_min

DEVICES=168

PORT=COM24
AVRDUDE=avrdude
TARGET=test
OBJECT_FILES=test.$*.o analog.$*.o assert.$*.o delay.$*.o lcd.$*.o buzzer.$*.o leds.$*.o motors.$*.o pushbuttons.$*.o qtr.$*.o
HEXES=$(foreach device, $(DEVICES), $(TARGET).$(device).hex)

all: $(HEXES)

clean:
	rm -f *.o *.hex *.obj *.hex

program-168: test.168.hex
	$(AVRDUDE) -p m168 -c avrisp2 -P $(PORT) -U flash:w:test.168.hex

# $< = prerequisite
# $@ = target
# $* = stem (percent)

$(HEXES): $(TARGET).%.hex: $(TARGET).%.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(foreach device, $(DEVICES), $(TARGET).$(device).obj): test.%.obj: test.%.o analog.%.o assert.%.o delay.%.o lcd.%.o buzzer.%.o leds.%.o motors.%.o pushbuttons.%.o qtr.%.o
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -lpololu_atmega$* -o $@
test.168.o: test.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
test.324p.o: test.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
analog.168.o: analog.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
analog.324p.o: analog.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
assert.168.o: assert.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
assert.324p.o: assert.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
delay.168.o: delay.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
delay.324p.o: delay.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
lcd.168.o: lcd.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
lcd.324p.o: lcd.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
buzzer.168.o: buzzer.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
buzzer.324p.o: buzzer.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
leds.168.o: leds.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
leds.324p.o: leds.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
motors.168.o: motors.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
motors.324p.o: motors.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
pushbuttons.168.o: pushbuttons.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
pushbuttons.324p.o: pushbuttons.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
qtr.168.o: qtr.c
	avr-gcc -mmcu=atmega168 -Wall -Os -c $< -o $@
qtr.324p.o: qtr.c
	avr-gcc -mmcu=atmega324p -Wall -Os -c $< -o $@
