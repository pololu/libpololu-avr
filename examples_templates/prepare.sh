#!/bin/sh

set -ue

example=$1
device=$2
mcu=$3
device_specific_macro=$4

# For Atmel Studio, change the MCU to ATmega324PA; otherwise it will give this error
# message when the user presses F5:
# dbg ProcessesService::launch() failed: Failed to start programming session before chiperase.
as_mcu=$3
if [ "$mcu" = atmega324p ]
then
  as_mcu=ATmega324PA
fi

example_template=examples_templates/$example
example_dir=examples/$device/$example

mkdir -p $example_dir

# Copy the source files.
cp -a $example_template/*.[ch] $example_dir/

# Create the Makefile using concatenation.
cat examples_templates/template_$device.mk $example_template/Makefile > $example_dir/Makefile

# Copy and edit the AVR Studio 4 project file.
AS4_DSM=
if [ $device_specific_macro ]
then
  AS4_DSM="s/<OPTIONSFORALL>/<OPTIONSFORALL>-D$device_specific_macro /"
fi
cat $example_template/*.aps | sed \
  -e "s/<PART>[^<]*<\/PART>/<PART>$mcu<\/PART>/" \
  -e "s/<LIB>libpololu[^<]*\.a<\/LIB>/<LIB>libpololu_$device.a<\/LIB>/" \
  -e "$AS4_DSM" \
  > $example_dir/$example.aps

# Copy and edit the AVR Studio 5 project file.

AS5_DSM=
if [ $device_specific_macro ]
then
  AS5_DSM="s/<avrgcc.compiler.symbols.DefSymbols><ListValues><\/ListValues><\/avrgcc.compiler.symbols.DefSymbols>/<avrgcc.compiler.symbols.DefSymbols><ListValues><Value>$device_specific_macro<\/Value><\/ListValues><\/avrgcc.compiler.symbols.DefSymbols>/"
fi
sed < examples_templates/test.cproj \
  -e "s/<avrdevice>[^<]*<\/avrdevice>/<avrdevice>$as_mcu<\/avrdevice>/" \
  -e "s/<avrgcc.common.Device>[^<]*<\/avrgcc.common.Device>/<avrgcc.common.Device>-mmcu=$as_mcu<\/avrgcc.common.Device>/" \
  -e "s/<Value>pololu_[^<]*<\/Value>/<Value>pololu_$device<\/Value>/" \
  -e "$AS5_DSM" \
  > $example_dir/$example.cproj

# TODO: we should think about editing the cproj file to refer to each .c file by name instead of
# using "*.c".  That will avoid the double-compilation problem that happens when a user adds a
# new .c file and tries to compile.

# Copy and edit the AVR Studio 5.1 solution file.
sed < examples_templates/test.atsln \
  -e "s/\"test/\"$example/g" \
  > $example_dir/$example.atsln
