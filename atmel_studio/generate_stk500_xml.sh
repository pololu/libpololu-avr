#!/bin/sh

# Generates the XML files that we need to add to Atmel Studio 6 to make
# it support programming some of the AVRs we use on our Orangutans.
# If these XML files are installed in
# C:\Program Files (x86)\Atmel\AVR Studio 6.2\tools\stk500\xml
# then new entries will appear in the Device dropdown box in the
# Device Programming dialog.
# These files are not needed in Atmel Studio 7.

set -ue

cd `dirname $0`

mkdir -p stk500_xml

for device in ATmega328P ATmega644P ATmega1284P
do
  cat > stk500_xml/${device}_stk500.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<avr-tools-part-file xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="../../schema/avr_tools_part_file.xsd">
  <devices>
    <device name="$device">
      <tools>
        <tool name="STK500" type="com.atmel.avrdbg.tool.stk500"/>
      </tools>
    </device>
  </devices>
</avr-tools-part-file>
EOF
done
