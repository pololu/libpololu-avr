#!/bin/sh

# Generates the zip files for our AVR Studio 5 templates.
# If these ZIP files are then installed in
# C:\Program Files (x86)\Atmel\AVR Studio 5.0\projecttemplates\Pololu Boards
# and the project template cache folder is deleted, then these
# templates will appear in AVR Studio

set -ue

cd `dirname $0`/templates_src
rm -f *.zip
D=../templates

mkdir -p $D

zip -MMjq $D/orangutan_svp_324.zip svp/svp_324.vstemplate svp/*.c template.avrgccproj svp/*.jpg
zip -MMjq $D/orangutan_svp_1284.zip svp/svp_1284.vstemplate svp/*.c template.avrgccproj svp/*.jpg

zip -MMjq $D/orangutan_x2_1284.zip x2/x2_1284.vstemplate x2/*.c x2/*.avrgccproj x2/*.jpg
zip -MMjq $D/orangutan_x2_644.zip x2/x2_644.vstemplate x2/*.c x2/*.avrgccproj x2/*.jpg

zip -MMjq $D/3pi_328.zip 3pi/3pi_328.vstemplate 3pi/*.c template.avrgccproj 3pi/*.jpg
zip -MMjq $D/3pi_168.zip 3pi/3pi_168.vstemplate 3pi/*.c template.avrgccproj 3pi/*.jpg

zip -MMjq $D/orangutan_sv_328.zip sv/sv_328.vstemplate sv/*.c template.avrgccproj sv/sv_328_preview.jpg sv/sv_icon.jpg
zip -MMjq $D/orangutan_sv_168.zip sv/sv_168.vstemplate sv/*.c template.avrgccproj sv/sv_168_preview.jpg sv/sv_icon.jpg

zip -MMjq $D/orangutan_lv_168.zip lv/lv_168.vstemplate lv/*.c template.avrgccproj lv/*.jpg

zip -MMjq $D/baby_orangutan_b_328.zip baby/baby_b_328.vstemplate baby/*.c template.avrgccproj baby/*.jpg
zip -MMjq $D/baby_orangutan_b_168.zip baby/baby_b_168.vstemplate baby/*.c template.avrgccproj baby/*.jpg
zip -MMjq $D/baby_orangutan_b_48.zip baby/baby_b_48.vstemplate baby/*.c template.avrgccproj baby/*.jpg
