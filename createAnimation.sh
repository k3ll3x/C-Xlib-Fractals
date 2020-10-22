#!/bin/bash

for i in {1..100}
do
	./fracimg;
	cp frac.ppm anim/$i.ppm;
done

cd anim;
convert -delay 10 -loop 0 *.ppm anim.gif;
#rm *.ppm;
xdg-open anim.gif;
