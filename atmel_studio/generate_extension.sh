#!/bin/sh

# This script generates the Atmel Studio extension file, extension.vsix.
# The extension can then be installed to Atmel Studio by double-clicking on it
# in Windows or by running vsixinstaller.exe, as we do in the library installer.
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

rm -Rf ProjectTemplates
mkdir -p ProjectTemplates/AVRGCC
cp -R templates ProjectTemplates/AVRGCC/Pololu

rm -f extension.vsix
zip -MMrq extension.vsix \[Content_Types\].xml extension.vsixmanifest ProjectTemplates icon.png preview.png
zip -MMjq extension.vsix ../LICENSE.txt
