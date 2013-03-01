#!/bin/sh

for f in `ls images/*.svg`
do
 #ksvgtopng 32 32 $f images/`basename $f .svg`.png
 inkscape -z --file=$f --export-png=images/`basename $f .svg`.png --export-width=32 --export-height=32
done

#for f in `ls images/*.png`
#do
# composite -compose overlay images/button.png $f $f
#done

uic -o buttons.h -embed buttons images/circle.png images/circle2.png images/close.png images/help.png images/keep_above.png images/keep_above_lit.png images/keep_below.png images/keep_below_lit.png images/maximize.png images/minimize.png images/restore.png images/splat.png images/unsplat.png images/window_background.png images/window_foreground.png

sed -e "s/factory/bfactory/" buttons.h >buttons.h.temp
mv buttons.h.temp buttons.h
