avrdude -p m168 -c avrisp2 -P COM6 -U flash:w:default\test.hex
sleep 10

