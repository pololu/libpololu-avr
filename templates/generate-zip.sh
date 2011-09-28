#!/bin/sh

# Generates the zip files for our AVR Studio 5 templates.
# If these ZIP files are then installed in
# C:\Program Files (x86)\Atmel\AVR Studio 5.0\projecttemplates\Pololu Boards
# and the project template cache folder is deleted, then these
# templates will appear in AVR Studio

set -ue

rm -f *.zip
zip -jq svp-324.zip svp/svp-324.vstemplate svp/*.c template.avrgccproj svp/*.jpg
zip -jq svp-1284.zip svp/svp-1284.vstemplate svp/*.c template.avrgccproj svp/*.jpg

zip -jq x2-1284.zip x2/x2-1284.vstemplate x2/*.c x2/*.avrgccproj x2/*.jpg
zip -jq x2-644.zip x2/x2-644.vstemplate x2/*.c x2/*.avrgccproj x2/*.jpg

zip -jq 3pi-328.zip 3pi/3pi-328.vstemplate 3pi/*.c template.avrgccproj 3pi/*.jpg
zip -jq 3pi-168.zip 3pi/3pi-168.vstemplate 3pi/*.c template.avrgccproj 3pi/*.jpg

zip -jq sv-328.zip sv/sv-328.vstemplate sv/*.c template.avrgccproj sv/sv-328_preview.jpg sv/sv_icon.jpg
zip -jq sv-168.zip sv/sv-168.vstemplate sv/*.c template.avrgccproj sv/sv-168_preview.jpg sv/sv_icon.jpg

zip -jq lv-168.zip lv/lv-168.vstemplate lv/*.c template.avrgccproj lv/*.jpg
