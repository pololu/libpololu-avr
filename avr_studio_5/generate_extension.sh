#!/bin/sh

# This script generates the AVR Studio 5 extension file, extension.vsix.
# Prerequisites: Execute generate_templates.sh.

set -ue         # Exit if there is an error.
cd `dirname $0` # Go to the right directory.

if [ $# != 0 ]
then
  DATE=$1
else
  DATE=`date +%y%m%d`
fi

sed "s/YYMMDD/$DATE/" extension.vsixmanifest.template > extension.vsixmanifest

rm -R ProjectTemplates
mkdir -p ProjectTemplates/AVRGCC
cp -R templates ProjectTemplates/AVRGCC/Pololu

rm extension.vsix
zip -MMrq extension.vsix \[Content_Types\].xml extension.vsixmanifest ProjectTemplates icon.png preview.png
zip -MMjq extension.vsix ../LICENSE.txt