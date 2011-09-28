#!/bin/bash
# Updates the pictures in our templates with the latest ones available
# on the Pololu website.

function pic {
  curl -sS http://www.pololu.com/picture/$1
}

pic 0J1893.32.jpg  > svp/svp_icon.jpg
pic 0J1882.200.jpg > svp/svp_preview.jpg

pic 0J103.32.jpg   > x2/x2_icon.jpg
pic 0J1287.200.jpg > x2/x2_preview.jpg

pic 0J2415.32.jpg  > 3pi/3pi_icon.jpg
pic 0J2416.200.jpg > 3pi/3pi_preview.jpg

pic 0J1279.32.jpg  > sv/sv_icon.jpg
pic 0J1139.200.jpg > sv/sv_168_preview.jpg
pic 0J1280.200.jpg > sv/sv_328_preview.jpg

pic 0J64.32.jpg    > lv/lv_icon.jpg
pic 0J65.200.jpg   > lv/lv_preview.jpg

pic 0J661.32.jpg   > baby/baby_icon.jpg
pic 0J666.200.jpg  > baby/baby_preview.jpg
