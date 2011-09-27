#!/bin/sh

# Generates the zip files for our AVR Studio 5 templates.
# If these ZIP files are then installed in
# C:\Program Files (x86)\Atmel\AVR Studio 5.0\projecttemplates\Pololu Boards
# and the project template cache folder is deleted, then these
# templates will appear in AVR Studio

set -ue

rm -f *.zip
zip -jq svp324.zip svp/svp324.vstemplate svp/*.c svp/*.avrgccproj svp/*.jpg
zip -jq svp1284.zip svp/svp1284.vstemplate svp/*.c svp/*.avrgccproj svp/*.jpg
