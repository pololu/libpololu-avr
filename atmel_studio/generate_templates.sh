#!/bin/sh

# Generates the zip files for our Atmel Studio templates.

set -ue

cd `dirname $0`/templates_src
D=../templates
rm -rf $D
mkdir -p $D

zip -MMjq $D/orangutan_svp_324.zip svp/svp_324.vstemplate svp/*.c template.cproj svp/*.jpg
zip -MMjq $D/orangutan_svp_1284.zip svp/svp_1284.vstemplate svp/*.c template.cproj svp/*.jpg

zip -MMjq $D/orangutan_x2_1284.zip x2/x2_1284.vstemplate x2/*.c template.cproj x2/*.jpg
zip -MMjq $D/orangutan_x2_644.zip x2/x2_644.vstemplate x2/*.c template.cproj x2/*.jpg

zip -MMjq $D/3pi_328.zip 3pi/3pi_328.vstemplate 3pi/*.c template.cproj 3pi/*.jpg
zip -MMjq $D/3pi_168.zip 3pi/3pi_168.vstemplate 3pi/*.c template.cproj 3pi/*.jpg

zip -MMjq $D/orangutan_sv_328.zip sv/sv_328.vstemplate sv/*.c template.cproj sv/sv_328_preview.jpg sv/sv_icon.jpg
zip -MMjq $D/orangutan_sv_168.zip sv/sv_168.vstemplate sv/*.c template.cproj sv/sv_168_preview.jpg sv/sv_icon.jpg

zip -MMjq $D/orangutan_lv_168.zip lv/lv_168.vstemplate lv/*.c template.cproj lv/*.jpg

zip -MMjq $D/baby_orangutan_b_328.zip baby/baby_b_328.vstemplate baby/*.c template.cproj baby/*.jpg
zip -MMjq $D/baby_orangutan_b_168.zip baby/baby_b_168.vstemplate baby/*.c template.cproj baby/*.jpg
zip -MMjq $D/baby_orangutan_b_48.zip baby/baby_b_48.vstemplate baby/*.c template.cproj baby/*.jpg
