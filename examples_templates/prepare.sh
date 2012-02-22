#!/bin/sh

set -ue

example=$1
device=$2
mcu=$3
device_specific_macro=$4

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
  -e "s/<avrdevice>[^<]*<\/avrdevice>/<avrdevice>$mcu<\/avrdevice>/" \
  -e "s/<Value>pololu_[^<]*<\/Value>/<Value>pololu_$device<\/Value>/" \
  -e "$AS5_DSM" \
  > $example_dir/$example.cproj

# Copy and edit the AVR Studio 5.0 solution file.
sed < examples_templates/test.avrsln \
  -e "s/\"test/\"$example/g" \
  > $example_dir/$example.avrsln

# Make an AVR Studio 5.1 solution file.
cp $example_dir/$example.avrsln $example_dir/$example.atsln